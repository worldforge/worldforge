//
// C++ Implementation: OgreResourceLoader
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2006
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.//
//
#include "OgreResourceLoader.h"
#include "framework/Tokeniser.h"
#include "services/config/ConfigService.h"
#include "model/ModelDefinitionManager.h"
#include "sound/XMLSoundDefParser.h"

#include "EmberOgreFileSystem.h"

#include "framework/TimedLog.h"
#include <OgreArchiveManager.h>
#include <Ogre.h>
#include <boost/algorithm/string.hpp>
#include <framework/FileSystemObserver.h>
#include <framework/MainLoopController.h>
#include <components/ogre/model/XMLModelDefinitionSerializer.h>
#include <squall/core/Repository.h>
#include "SquallArchive.h"
#include "squall/core/Difference.h"
#include <OgreScriptCompiler.h>

namespace {
void refreshShader(const std::filesystem::path& path, const std::string& group) {
	//When a shader file changes, we need to go through all GpuPrograms and find any that uses that file.
	//Multiple GPU Programs can be using the same shader, through the usage of pre-processor directives.
	//Once we've found that there's a matching GPU program, we'll reload that and then we need to go through _all_ materials,
	//and look through all techniques and all of their passes, to find if they refer to this changed GPU program, and if so reload it.
	//Hopefully this is quick, but if not we might need to offload it to a background task, however that will work.
	for (const auto& gpuProgramEntry: Ogre::GpuProgramManager::getSingleton().getResourceIterator()) {
		if (gpuProgramEntry.second->getGroup() == group) {
			auto gpuProgram = dynamic_cast<Ogre::GpuProgram*>(gpuProgramEntry.second.get());
			if (gpuProgram) {
				if (gpuProgram->getSourceFile() == path.string()) {
					if (gpuProgram->isReloadable()) {
						Ember::logger->info("Reloading GPU program {} since the source file at {} was changed.", gpuProgram->getName(), gpuProgram->getSourceFile());
						gpuProgram->reload();
						for (const auto& materialEntry: Ogre::MaterialManager::getSingleton().getResourceIterator()) {
							auto material = dynamic_cast<Ogre::Material* >(materialEntry.second.get());
							if (material) {
								if (material->isLoaded()) {
									for (auto technique: material->getTechniques()) {
										for (auto pass: technique->getPasses()) {
											for (std::uint8_t programTypeIndex = 0; programTypeIndex < Ogre::GPT_COUNT; ++programTypeIndex) {
												auto programType = static_cast<Ogre::GpuProgramType>(programTypeIndex);//Danger here, with the unsafe enum casting and all. It's dirty.
												if (pass->hasGpuProgram(programType)) {
													auto usedProg = pass->getGpuProgram(programType);
													if (usedProg) {
														if (usedProg.get() == gpuProgram) {
															Ember::logger->info("Reloading material {} since the GPU program at {} was changed.", material->getName(),
																				 gpuProgram->getSourceFile());
															material->reload();
															break;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					} else {
						Ember::logger->warn("GPU program {}, with source path {}, was updated from the server, but it's not reloadable. So it won't be reloaded.", gpuProgram->getName(),
											gpuProgram->getSourceFile());
					}
				}
			}
		}
	}
}

void parseScript(Ogre::ScriptLoader& scriptLoader, const std::filesystem::path& path, std::string group) {
	try {
		auto stream = Ogre::ResourceGroupManager::getSingleton().openResource(path.generic_string(), group, nullptr, false);
		if (stream) {
			scriptLoader.parseScript(stream, group);
		}
	} catch (const std::exception& ex) {
		Ember::logger->error("Error when parsing changed file '{}' in group '{}': {}", path.generic_string(), group, ex.what());
	}
}

/**
 * Refresh an updated model definition, either by just adding it if it doesn't already exists, or by updating it and reloading all instances.
 * @param fullPath The full path to the definition.
 */
void refreshModelDefinition(const std::filesystem::path& relativePath, const std::string& group) {
	auto stream = Ogre::ResourceGroupManager::getSingleton().openResource(relativePath.generic_string(), group, nullptr, false);
	if (stream) {
		auto& modelDefMgr = Ember::OgreView::Model::ModelDefinitionManager::getSingleton();
		Ember::OgreView::Model::XMLModelDefinitionSerializer serializer;

		auto modelDef = serializer.parseScript(stream);
		if (modelDef) {
			auto existingDef = modelDefMgr.getByName(relativePath.generic_string());
			//Model definition doesn't exist, just add it.
			if (!existingDef) {
				modelDefMgr.addDefinition(relativePath.generic_string(), modelDef);
			} else {
				//otherwise update existing
				existingDef->moveFrom(std::move(*modelDef));
				existingDef->reloadAllInstances();
			}
		}
	}
}

void processChangedOrNew(const std::filesystem::path& path, std::string group) {
	auto reloadOrAddResource = [&](Ogre::ResourceManager& resourceManager, const std::string& resourceName) {
		if (resourceManager.resourceExists(resourceName, group)) {
			auto resource = resourceManager.getResourceByName(resourceName, group);
			if (resource->isLoaded() || resource->isPrepared()) {
				try {
					resource->reload();
				} catch (const std::exception& e) {
					Ember::logger->error("Could not reload resource '{}' of type '{}': {}", resourceName, resourceManager.getResourceType(), e.what());
				}
			}
		} else {
			//Add resource
			Ogre::ResourceGroupManager::getSingleton().declareResource(resourceName, resourceManager.getResourceType(), group);
			resourceManager.createResource(resourceName, group);
		}
	};


	auto extension = path.extension();

	if (extension == ".png" || extension == ".dds" || extension == ".jpg" || extension == ".jpeg") {
		reloadOrAddResource(Ogre::TextureManager::getSingleton(), path.generic_string());
	} else if (extension == ".mesh") {
		reloadOrAddResource(Ogre::MeshManager::getSingleton(), path.generic_string());
	} else if (extension == ".skeleton") {
		reloadOrAddResource(Ogre::SkeletonManager::getSingleton(), path.generic_string());
	} else if (extension == ".frag" || extension == ".vert" || extension == ".glsl") {
		refreshShader(path.generic_string(), group);
	} else if (extension == ".program") {
		parseScript(Ogre::ScriptCompilerManager::getSingleton(), path.generic_string(), group);
	} else if (extension == ".material") {
		parseScript(Ogre::MaterialManager::getSingleton(), path.generic_string(), group);
	} else if (extension == ".modeldef") {
		refreshModelDefinition({path}, group);
	}
}

}

namespace Ember::OgreView {

struct EmberResourceLoadingListener : public Ogre::ResourceLoadingListener {
	Ogre::DataStreamPtr resourceLoading(const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource) override {
		return {};
	}

	void resourceStreamOpened(const Ogre::String& name, const Ogre::String& group, Ogre::Resource* resource, Ogre::DataStreamPtr& dataStream) override {

	}

	bool resourceCollision(Ogre::Resource* resource, Ogre::ResourceManager* resourceManager) override {
		logger->debug("Resource '{}' already exists in group '{}' for type '{}'.", resource->getName(), resource->getGroup(), resourceManager->getResourceType());
		if (resourceManager->getResourceType() == "Material") {

			//If a material, update the old version once the new one has been compiled (hence the need for "runOnMainThread".
			//Note that this only works the first time a material is updated.
			Ogre::MaterialPtr existingMaterial = Ogre::MaterialManager::getSingleton().getByName(resource->getName(), resource->getGroup());

			MainLoopController::getSingleton().getEventService().runOnMainThread([=]() {
				Ogre::MaterialPtr oldMat = existingMaterial;
				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(existingMaterial->getName(), existingMaterial->getGroup());
				if (mat) {
					mat->copyDetailsTo(oldMat);
					oldMat->load();
				} else {
					logger->warn("Material '{}' does not exist anymore in group '{}'.", existingMaterial->getName(), existingMaterial->getGroup());
				}
			});
		}

		resourceManager->remove(resource->getName(), resource->getGroup());
		return true;
	}
};

OgreResourceLoader::OgreResourceLoader(Squall::Repository repository) :
		UnloadUnusedResources("unloadunusedresources", this, "Unloads any unused resources."),
		mFileSystemArchiveFactory(std::make_unique<FileSystemArchiveFactory>()),
		mSquallArchiveFactory(std::make_unique<SquallArchiveFactory>(repository)),
		mLoadingListener(std::make_unique<EmberResourceLoadingListener>()),
		mRepository(repository) {
}

OgreResourceLoader::~OgreResourceLoader() {
	//Don't deregister mLoadingListener, since this destructor needs to be called after OGRE has been shut down, and there won't be any ResourceGroupManager
	//available by then.
	for (auto& path: mResourceRootPaths) {
		Ember::FileSystemObserver::getSingleton().remove_directory(path);
	}
}

void OgreResourceLoader::initialize() {

	//We can only add an archive factory, there's no method for removing it. Thus an instance of this needs to survive longer than Ogre itself.
	Ogre::ArchiveManager::getSingleton().addArchiveFactory(mFileSystemArchiveFactory.get());
	Ogre::ArchiveManager::getSingleton().addArchiveFactory(mSquallArchiveFactory.get());

	Ogre::ResourceGroupManager::getSingleton().setLoadingListener(mLoadingListener.get());

	auto& configSrv = ConfigService::getSingleton();

	if (configSrv.itemExists("media", "extraresourcelocations")) {
		varconf::Variable const resourceConfigFilesVar = configSrv.getValue("media", "extraresourcelocations");
		std::string const resourceConfigFiles = resourceConfigFilesVar.as_string();
		mExtraResourceLocations = Tokeniser::split(resourceConfigFiles, ";");
	}
}

void OgreResourceLoader::runCommand(const std::string& command, const std::string& args) {
	if (UnloadUnusedResources == command) {
		unloadUnusedResources();
	}
}

void OgreResourceLoader::unloadUnusedResources() {
	TimedLog const l("Unload unused resources.");
	Ogre::ResourceGroupManager& resourceGroupManager(Ogre::ResourceGroupManager::getSingleton());

	auto resourceGroups = resourceGroupManager.getResourceGroups();
	for (const auto& resourceGroup: resourceGroups) {
		resourceGroupManager.unloadUnreferencedResourcesInGroup(resourceGroup, false);
	}
}

bool OgreResourceLoader::addSharedMedia(const std::string& path, const std::string& type, const std::string& section) {
	auto sharedMediaPath = ConfigService::getSingleton().getSharedDataDirectory();
	return addResourceDirectory(sharedMediaPath / path, type, section, OnFailure::Throw);
}

bool OgreResourceLoader::addUserMedia(const std::string& path, const std::string& type, const std::string& section) {
	auto userMediaPath = ConfigService::getSingleton().getUserMediaDirectory();

	return addResourceDirectory(userMediaPath / path, type, section, OnFailure::Ignore);
}

bool OgreResourceLoader::addResourceDirectory(const std::filesystem::path& path,
											  const std::string& type,
											  const std::string& section,
											  OnFailure onFailure) {
	if (std::filesystem::is_directory(path)) {
		logger->debug("Adding dir {}", path.string());
		try {
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path.string(), type, section, true);
			mResourceRootPaths.emplace_back(path.string());
			observeDirectory(path, section);

			return true;
		} catch (const std::exception&) {
			switch (onFailure) {
				case OnFailure::Ignore:
					break;
				case OnFailure::Report:
					logger->error("Couldn't load {}. Continuing as if nothing happened.", path.string());
					break;
				case OnFailure::Throw:
					throw Ember::Exception(fmt::format(
							"Could not load from required directory '{}'. This is fatal and Ember will shut down. The probable cause for this error is that you haven't properly installed all required media.",
							path.string()));
			}
		}
	} else {
		switch (onFailure) {
			case OnFailure::Ignore:
				break;
			case OnFailure::Report:
				logger->error("Couldn't find resource directory {}", path.string());
				break;
			case OnFailure::Throw:
				throw Ember::Exception(fmt::format(
						"Could not find required directory '{}'. This is fatal and Ember will shut down. The probable cause for this error is that you haven't properly installed all required media.",
						path.string()));
		}
	}
	return false;
}

void OgreResourceLoader::loadBootstrap() {
	addSharedMedia("data/ui", "EmberFileSystem", "UI");

	addSharedMedia("data/assets", "EmberFileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	addSharedMedia("OGRE/Media/Main", "EmberFileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	addSharedMedia("OGRE/Media/RTShaderLib", "EmberFileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	addUserMedia("media/assets", "EmberFileSystem", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}

void OgreResourceLoader::loadGui() {
	addSharedMedia("gui", "EmberFileSystem", "UI");
	addUserMedia("gui", "EmberFileSystem", "UI");
}

void OgreResourceLoader::loadGeneral() {


	//Lua scripts
	addSharedMedia("scripting", "EmberFileSystem", "Scripting");
	addUserMedia("scripting", "EmberFileSystem", "Scripting");

	//Model definitions, terrain definitions, sound definitions and entity mappings
	//TODO: remove
	addSharedMedia("data/dural", "EmberFileSystem", "Data");
	addUserMedia("data", "EmberFileSystem", "Data");

	//The Caelum component
	addSharedMedia("data/caelum", "EmberFileSystem", "Caelum");

	//Entity recipes
	addSharedMedia("data/entityrecipes", "EmberFileSystem", "EntityRecipes");
	addUserMedia("entityrecipes", "EmberFileSystem", "EntityRecipes");

	//End with adding any extra defined locations.
	for (auto& location: mExtraResourceLocations) {
		addResourceDirectory(location, "EmberFileSystem", "Extra", OnFailure::Report);
	}


}

void OgreResourceLoader::preloadMedia() {
	// resource groups to be loaded
	const char* resourceGroup[] = {"General", "Data"};

	for (auto& group: resourceGroup) {
		try {
			Ogre::ResourceGroupManager::getSingleton().loadResourceGroup(group);
		} catch (const std::exception& ex) {
			logger->error("An error occurred when preloading media: {}", ex.what());
		}
	}
}

void OgreResourceLoader::observeDirectory(const std::filesystem::path& path, std::string group) {
	try {
		FileSystemObserver::getSingleton().add_directory(path, [group](const FileSystemObserver::FileSystemEvent& event) {
			auto& ev = event.ev;
			//Skip if it's not a file. This also means that we won't catch deletion of files. That's ok for now; but perhaps we need to revisit this.
			if (!std::filesystem::is_regular_file(ev.path)) {
				return;
			}
			logger->debug("Resource changed {} {}", ev.path.string(), ev.type_cstr());

			if (ev.type == boost::asio::dir_monitor_event::modified) {
				try {
					if (std::filesystem::file_size(ev.path) == 0) {
						return;
					}
				} catch (...) {
					//Could not find file, just return
					return;
				}
			}

			auto startsWith = [](const std::filesystem::path& root, std::filesystem::path aPath) {
				while (!aPath.empty()) {
					if (aPath == root) {
						return true;
					}
					aPath = aPath.parent_path();
				}
				return false;
			};

			auto locations = Ogre::ResourceGroupManager::getSingleton().listResourceLocations(group);
			for (auto& location: *locations) {
				std::filesystem::path locationDirectory(location);
				if (startsWith(locationDirectory, ev.path)) {
					auto relative = std::filesystem::relative(ev.path, locationDirectory);

					processChangedOrNew({relative.string()}, group);
					break;
				}
			}

		});
	} catch (...) {
		//Ignore errors
	}

}

bool OgreResourceLoader::addMedia(const std::string& path, const std::string& resourceGroup) {
	return addSharedMedia("media/" + path, "EmberFileSystem", resourceGroup);
}

bool OgreResourceLoader::addSquallMedia(Squall::Signature signature) {

	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(signature.str(),
																   "Squall",
																   "world",
																   true);
	mSquallSignature = signature;

	return true;
}

void OgreResourceLoader::replaceSquallMedia(Squall::Signature signature) {
	if (mSquallSignature) {
		auto newManifestOptional = mRepository.fetchManifest(signature);
		auto oldManifestOptional = mRepository.fetchManifest(*mSquallSignature);
		if (newManifestOptional.manifest && oldManifestOptional.manifest) {
			auto difference = Squall::resolveDifferences(mRepository, Squall::DifferenceRequest{.oldManifest = *oldManifestOptional.manifest, .newManifest = *newManifestOptional.manifest});

			auto& resourceGroupManager = Ogre::ResourceGroupManager::getSingleton();
			auto squallArchive = static_cast<SquallArchive*>(resourceGroupManager.getResourceLocationList("world").front().archive);
			squallArchive->setRootSignature(signature);


			for (auto& entry: difference.newEntries) {
				processChangedOrNew(entry.path, "world");
			}

			for (auto& entry: difference.alteredEntries) {
				processChangedOrNew(entry.change.path, "world");
			}

			//How should we handle removed entries? Normally if something isn't used it should be removed as part of cleanup, so perhaps there's no need to actively unload it?
		}

	}
}
}
