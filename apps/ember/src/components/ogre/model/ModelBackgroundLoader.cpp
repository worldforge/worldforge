//
// C++ Implementation: ModelBackgroundLoader
//
// Description:
//
//
// Author: Erik Ogenvik <erik@ogenvik.org>, (C) 2009
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
#include "ModelBackgroundLoader.h"
#include "Model.h"
#include "framework/Log.h"
#include "bytesize/bytesize.hh"

#include <OgreSubMesh.h>
#include <OgreMaterialManager.h>
#include <OgreMeshManager.h>
#include <OgreTextureManager.h>

#include <framework/MainLoopController.h>
#include <Ogre.h>

namespace Ember::OgreView::Model {

ModelBackgroundLoader::ModelBackgroundLoader(ModelDefinition& modelDefinition) :
		mModelDefinition(modelDefinition),
		mResourcesBeingLoadingInBackground(0),
		mState(LS_UNINITIALIZED),
		mSubModelLoadingIndex(0) {
}

ModelBackgroundLoader::~ModelBackgroundLoader() = default;

bool ModelBackgroundLoader::poll() {
	bool result = performLoading();
	if (result) {
		mModelDefinition.notifyAssetsLoaded();
		return true;
	}

	if (mResourcesBeingLoadingInBackground == 0) {
		MainLoopController::getSingleton().getEventService().runOnMainThread([this]() {
			this->poll();
		}, mIsActive);
	}
	return false;

}

void ModelBackgroundLoader::prepareMaterialInBackground(const std::string& materialName) {
	if (!materialName.empty()) {
		auto materialPtr = Ogre::MaterialManager::getSingleton().createOrRetrieve(materialName, "world").first;
		if (materialPtr) {
			mMaterialsToLoad.insert(std::static_pointer_cast<Ogre::Material>(materialPtr));
			if (!materialPtr->isPrepared() && !materialPtr->isLoading() && !materialPtr->isLoaded()) {
				mResourcesBeingLoadingInBackground++;
				auto task = std::make_shared<std::packaged_task<void()>>(
						[materialPtr, this]() {
							materialPtr->prepare(true);
							Ogre::Root::getSingleton().getWorkQueue()->addMainThreadTask([materialPtr, this]() {
								materialPtr->_firePreparingComplete();
								this->mResourcesBeingLoadingInBackground--;
								this->poll();
							});
						});
				Ogre::Root::getSingleton().getWorkQueue()->addTask([task]() { (*task)(); });
			}
		}
	}
}

void ModelBackgroundLoader::prepareMeshInBackground(const std::string& meshName) {

	if (!meshName.empty()) {
		auto meshPtr = Ogre::MeshManager::getSingleton().createOrRetrieve(meshName, "world").first;
		if (meshPtr) {
			if (!meshPtr->isPrepared() && !meshPtr->isLoading() && !meshPtr->isLoaded()) {
				mResourcesBeingLoadingInBackground++;
				auto task = std::make_shared<std::packaged_task<void()>>(
						[meshPtr, this]() {
							meshPtr->prepare(true);
							Ogre::Root::getSingleton().getWorkQueue()->addMainThreadTask([meshPtr, this]() {
								meshPtr->_firePreparingComplete();
								this->mResourcesBeingLoadingInBackground--;
								this->poll();
							});
						});
				Ogre::Root::getSingleton().getWorkQueue()->addTask([task]() { (*task)(); });
			}
		}
	}
}

void ModelBackgroundLoader::prepareTextureInBackground(const std::string& textureName) {
	if (!textureName.empty()) {
		auto texturePtr = Ogre::TextureManager::getSingleton().getByName(textureName, "world");
		if (texturePtr) {
			mTexturesToLoad.insert(texturePtr->getName());
			if (!texturePtr->isPrepared() && !texturePtr->isLoading() && !texturePtr->isLoaded()) {
				mResourcesBeingLoadingInBackground++;
				auto task = std::make_shared<std::packaged_task<void()>>(
						[texturePtr, this]() {
							texturePtr->prepare(true);
							Ogre::Root::getSingleton().getWorkQueue()->addMainThreadTask([texturePtr, this]() {
								texturePtr->_firePreparingComplete();
								this->mResourcesBeingLoadingInBackground--;
								this->poll();
							});
						});
				Ogre::Root::getSingleton().getWorkQueue()->addTask([task]() { (*task)(); });
			}
		}
	}
}


bool ModelBackgroundLoader::performLoading() {
//	TimedLog log("performLoading " + mModelDefinition.getName() + " state: " + std::to_string(mState));
	if (mState == LS_UNINITIALIZED) {
		//Start to load the meshes
		for (auto& subModel: mModelDefinition.getSubModelDefinitions()) {
			prepareMeshInBackground(subModel.meshName);
		}
		mState = LS_MESH_PREPARING;
		return false;
	} else if (mState == LS_MESH_PREPARING) {
		if (areAllTicketsProcessed()) {
			mState = LS_MESH_PREPARED;
			return performLoading();
		}
	} else if (mState == LS_MESH_PREPARED) {
		auto& submodelDefinitions = mModelDefinition.getSubModelDefinitions();
		while (mSubModelLoadingIndex < submodelDefinitions.size()) {
			auto submodelDef = submodelDefinitions.at(mSubModelLoadingIndex);
			mSubModelLoadingIndex++;
			Ogre::MeshPtr meshPtr = Ogre::static_pointer_cast<Ogre::Mesh>(
					Ogre::MeshManager::getSingleton().getByName(submodelDef.meshName, "world"));
			if (meshPtr) {
				if (!meshPtr->isLoaded()) {
					try {
						meshPtr->load();
						logger->debug("Loaded mesh in main thread: {} Memory used: {}", meshPtr->getName(), bytesize::bytesize(meshPtr->getSize()));
					} catch (const std::exception& ex) {
						logger->error("Could not load the mesh {} when loading model {}: {}", meshPtr->getName(), mModelDefinition.getOrigin(), ex.what());
					}
					return false;
				}
			}
		}
		mState = LS_MESH_LOADING;
		return false;
	} else if (mState == LS_MESH_LOADING) {
		if (areAllTicketsProcessed()) {
			mState = LS_MESH_LOADED;
			return performLoading();
		}
	} else if (mState == LS_MESH_LOADED) {

		for (auto& submodelDef: mModelDefinition.getSubModelDefinitions()) {

			Ogre::MeshPtr meshPtr = Ogre::static_pointer_cast<Ogre::Mesh>(
					Ogre::MeshManager::getSingleton().getByName(submodelDef.meshName, "world"));
			if (meshPtr) {
				if (meshPtr->isLoaded()) {
					for (auto submesh: meshPtr->getSubMeshes()) {
						prepareMaterialInBackground(submesh->getMaterialName());
					}
				}
			}
			for (auto& partDef: submodelDef.getPartDefinitions()) {
				if (!partDef.getSubEntityDefinitions().empty()) {
					for (auto& subEntityDef: partDef.getSubEntityDefinitions()) {
						prepareMaterialInBackground(subEntityDef.materialName);
					}
				}
			}
		}
		mState = LS_PARTICLE_SYSTEM_PREPARING;
		return false;
	} else if (mState == LS_PARTICLE_SYSTEM_PREPARING) {
		for (auto& particleSystemDef: mModelDefinition.mParticleSystems) {
			auto particleSystemTemplate = Ogre::ParticleSystemManager::getSingleton().getTemplate(particleSystemDef.Script);
			if (particleSystemTemplate) {
				prepareMaterialInBackground(particleSystemTemplate->getMaterialName());
			}
		}
		mState = LS_MATERIAL_PREPARING;
		return false;
	} else if (mState == LS_MATERIAL_PREPARING) {
		if (areAllTicketsProcessed()) {
			mState = LS_MATERIAL_PREPARED;
			return performLoading();
		}
		return false;
	} else if (mState == LS_MATERIAL_PREPARED) {
		for (auto& materialPtr: mMaterialsToLoad) {
			if (materialPtr) {
				//We can't call on "getSupportedTechniques()" here since that gets populated first when "load()" is called
				for (auto* tech: materialPtr->getTechniques()) {
					for (auto* pass: tech->getPasses()) {
						for (auto* tus: pass->getTextureUnitStates()) {
							if (tus->getContentType() == Ogre::TextureUnitState::ContentType::CONTENT_NAMED) {
								unsigned int frames = tus->getNumFrames();
								for (unsigned int i = 0; i < frames; ++i) {
									const auto& textureName = tus->getFrameTextureName(i);
									prepareTextureInBackground(textureName);
								}
							}
						}
					}
				}
			}
		}
		mState = LS_TEXTURE_PREPARING;
		return false;
	} else if (mState == LS_TEXTURE_PREPARING) {
		if (areAllTicketsProcessed()) {
			mState = LS_TEXTURE_PREPARED;
			return performLoading();
		}
	} else if (mState == LS_TEXTURE_PREPARED) {
		if (!mTexturesToLoad.empty()) {
			auto I = mTexturesToLoad.begin();
			const std::string& textureName = *I;
			mTexturesToLoad.erase(I);
			auto texture = Ogre::TextureManager::getSingleton().getByName(textureName, "world");
			if (texture && !texture->isLoaded()) {
				try {
					if (!texture->isPrepared()) {
						logger->warn("Texture was not prepared: {}", texture->getName());
					}
					texture->load();
					logger->debug("Loaded texture in main thread: {} Memory used: {}", texture->getName(), bytesize::bytesize(texture->getSize()));
				} catch (const std::exception& e) {
					logger->warn("Error when loading texture {}: {}", texture->getName(), e.what());
				}
				return false;
			}
		}
		mState = LS_MATERIAL_LOADING;
		return false;
	} else if (mState == LS_MATERIAL_LOADING) {
		if (areAllTicketsProcessed()) {
			if (!mMaterialsToLoad.empty()) {
				auto I = mMaterialsToLoad.begin();
				auto material = *I;
				mMaterialsToLoad.erase(I);
				if (!material->isLoaded()) {
					try {
						material->load();
						logger->debug("Loaded material in main thread: {}", material->getName());
					} catch (const std::exception& e) {
						logger->warn("Error when loading material {}: {}", material->getName(), e.what());
					}
					return false;
				}
			}

			mState = LS_DONE;
			return true;
		}
	} else {
		return true;
	}
	return false;
}

bool ModelBackgroundLoader::areAllTicketsProcessed() const {
	return mResourcesBeingLoadingInBackground == 0;
}

ModelBackgroundLoader::LoadingState ModelBackgroundLoader::getState() const {
	return mState;
}


}



