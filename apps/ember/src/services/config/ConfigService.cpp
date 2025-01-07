/*
    Copyright (C) 2002  Miguel Guzman Miranda [Aglanor]
                        Joel Schander         [nullstar]
                        Erik Ogenvik

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ConfigService.h"
#include "framework/Log.h"
#include "framework/ConsoleBackend.h"

#ifdef _WIN32
#include "platform/platform_windows.h"

//we need this for the PathRemoveFileSpec(...) method
#include <shlwapi.h>
#include <Shlobj.h>
#endif

#include <fstream>
#include <filesystem>

#if !defined(__APPLE__) && !defined(_WIN32)

#include <basedir.h>

#endif

// From sear
#ifdef __APPLE__

#include <CoreFoundation/CFBundle.h>
#include <CoreServices/CoreServices.h>

using namespace Ember;

std::string getBundleResourceDirPath()
{
	/* the following code looks for the base package directly inside
	the application bundle. This can be changed fairly easily by
	fiddling with the code below. And yes, I know it's ugly and verbose.
	*/
	std::string result;
	CFBundleRef appBundle = CFBundleGetMainBundle();
	CFURLRef resUrl = CFBundleCopyResourcesDirectoryURL(appBundle);
	CFURLRef absResUrl = CFURLCopyAbsoluteURL(resUrl);

	// now convert down to a path, and the a c-string
	CFStringRef path = CFURLCopyFileSystemPath(absResUrl, kCFURLPOSIXPathStyle);
	result = CFStringGetCStringPtr(path, CFStringGetSystemEncoding());

	CFRelease(resUrl);
	CFRelease(absResUrl);
	CFRelease(path);
	return result;
}

std::string getAppSupportDirPath()
{
	FSRef fs;
	OSErr err = FSFindFolder(kUserDomain, kApplicationSupportFolderType, true, &fs);
	if (err != noErr) {
		logger->error("error doing FindFolder");
		return std::string();
	}

	CFURLRef dirURL = CFURLCreateFromFSRef(kCFAllocatorSystemDefault, &fs);
	char fsRepr[1024];
	if (!CFURLGetFileSystemRepresentation(dirURL, true, (UInt8*) fsRepr, 1024)) {
		std::cerr << "error invoking CFURLGetFileSystemRepresentation" << std::endl;
		return std::string();
	}

	CFRelease(dirURL);
	return std::string(fsRepr);
}

#endif


namespace Ember {

ConfigService::ConfigService(std::filesystem::path prefix) :
#if !defined(__APPLE__) && !defined(_WIN32)
		mBaseDirHandle{},
#endif
		mSharedDataDir(""),
		mEtcDir(""),
		mHomeDir(""),
		mPrefix(std::move(prefix)) {
#ifdef _WIN32
	char cwd[512];
	//get the full path for the current executable
	GetModuleFileName ( 0, cwd, 512 );

	//use this utility function for removing the file part
	PathRemoveFileSpec ( cwd );
	std::filesystem::path baseDir( cwd );
	mSharedDataDir = baseDir / ".." / "share" / "ember";
	mEtcDir = baseDir / ".." / "etc" / "ember";
	mPluginDir = baseDir / ".." / "lib" / "ember" / "widgets";

#endif

#if !defined(__APPLE__) && !defined(_WIN32)
	if (mPrefix.empty()) {
		mSharedDataDir = EMBER_DATADIR "/ember/";
		mEtcDir = EMBER_SYSCONFDIR "/ember/";
		mPluginDir = EMBER_LIBDIR "/ember/widgets";
	} else {
		mSharedDataDir = mPrefix / "share" / "ember";
		//CMake handles the install prefix of "/usr" differently, in that it puts config files in "/etc" instead of "/usr/etc".
		//We need to detect this.
		if (std::string(PREFIX) == "/usr") {
			mEtcDir = mPrefix.parent_path() / "etc" / "ember";
		} else {
			mEtcDir = mPrefix / "etc" / "ember";
		}
		//Get the last directory from the libdir
		auto libdirectory = *(--std::filesystem::path(EMBER_LIBDIR).end());
		mPluginDir = mPrefix / libdirectory / "ember" / "widgets";
	}
	xdgInitHandle(&mBaseDirHandle);
	logger->info("Setting config directory to '{}'", mEtcDir.string());
#endif
	mGlobalConfig.sige.connect(sigc::mem_fun(*this, &ConfigService::configError));
	mGlobalConfig.sigv.connect(sigc::mem_fun(*this, &ConfigService::updatedConfig));
	mUserConfig.sige.connect(sigc::mem_fun(*this, &ConfigService::configError));
	mUserConfig.sigv.connect(sigc::mem_fun(*this, &ConfigService::updatedConfig));
	mInstanceConfig.sige.connect(sigc::mem_fun(*this, &ConfigService::configError));
	mInstanceConfig.sigv.connect(sigc::mem_fun(*this, &ConfigService::updatedConfig));
}

ConfigService::~ConfigService() {
#if !defined(__APPLE__) && !defined(_WIN32)
	xdgWipeHandle(&mBaseDirHandle);
#endif
}

const std::filesystem::path& ConfigService::getPrefix() const {
	return mPrefix;
}


void ConfigService::setHomeDirectory(const std::string& path) {
	mHomeDir = path;
}

ConfigService::SectionMap ConfigService::getSection(const std::string& sectionName) {
	SectionMap combinedSection;

	if (mInstanceConfig.findSection(sectionName)) {
		auto section = mInstanceConfig.getSection(sectionName);
		if (section) {
			combinedSection.insert(section->begin(), section->end());
		}
	}
	if (mCommandLineConfig.findSection(sectionName)) {
		auto section = mCommandLineConfig.getSection(sectionName);
		if (section) {
			combinedSection.insert(section->begin(), section->end());
		}
	}
	if (mUserConfig.findSection(sectionName)) {
		auto section = mUserConfig.getSection(sectionName);
		if (section) {
			combinedSection.insert(section->begin(), section->end());
		}
	}
	if (mGlobalConfig.findSection(sectionName)) {
		auto section = mGlobalConfig.getSection(sectionName);
		if (section) {
			combinedSection.insert(section->begin(), section->end());
		}
	}
	return combinedSection;
}


varconf::Variable ConfigService::getValue(const std::string& section, const std::string& key) const {
	if (mInstanceConfig.findItem(section, key)) {
		return mInstanceConfig.getItem(section, key);
	}
	if (mCommandLineConfig.findItem(section, key)) {
		return mCommandLineConfig.getItem(section, key);
	}
	if (mUserConfig.findItem(section, key)) {
		return mUserConfig.getItem(section, key);
	}
	return mGlobalConfig.getItem(section, key);
}

bool ConfigService::getValue(const std::string& section, const std::string& key, varconf::Variable& value) const {
	if (mInstanceConfig.findItem(section, key)) {
		value = mInstanceConfig.getItem(section, key);
		return true;
	}
	if (mCommandLineConfig.findItem(section, key)) {
		value = mCommandLineConfig.getItem(section, key);
		return true;
	}
	if (mUserConfig.findItem(section, key)) {
		value = mUserConfig.getItem(section, key);
		return true;
	}
	if (mGlobalConfig.findItem(section, key)) {
		value = mGlobalConfig.getItem(section, key);
		return true;
	}
	return false;
}

void ConfigService::setValue(const std::string& section, const std::string& key, const varconf::Variable& value, int iscope) {
	auto scope = static_cast<varconf::Scope>(iscope);

	switch (scope) {
		case varconf::GLOBAL:
			mGlobalConfig.setItem(section, key, value);
			break;
		case varconf::USER:
			mUserConfig.setItem(section, key, value);
			break;
		case varconf::INSTANCE:
			mInstanceConfig.setItem(section, key, value);
			break;
	}
}

bool ConfigService::isItemSet(const std::string& section, const std::string& key, const std::string& value) const {
	return (hasItem(section, key) && getValue(section, key) == value);
}

bool ConfigService::itemExists(const std::string& section, const std::string& key) const {
	return hasItem(section, key);
}

bool ConfigService::hasItem(const std::string& section, const std::string& key) const {
	return mGlobalConfig.find(section, key) || mUserConfig.find(section, key) || mInstanceConfig.find(section, key) || mCommandLineConfig.find(section, key);
}

bool ConfigService::loadSavedConfig(const std::string& filename, const StringConfigMap& commandLineSettings) {
	auto path = getSharedConfigDirectory() / filename;
	logger->info("Loading shared config file from {}.", path.string());
	bool success = mGlobalConfig.readFromFile(path.string(), varconf::GLOBAL);
	auto userConfigPath = getHomeDirectory(BaseDirType_CONFIG) / filename;
	std::ifstream file(userConfigPath.c_str());
	if (!file.fail()) {
		logger->info("Loading user config file from {}.", userConfigPath.string());
		try {
			mUserConfig.parseStream(file, varconf::USER);
		}
		catch (varconf::ParseError& p) {
			logger->error("Error loading user config file: {}", p.what());
			success = false;
		}
	} else {
		logger->info("Could not find any user config file.");
	}

	//after loading the config from file, override with command time settings
	for (const auto& commandLineSetting: commandLineSettings) {
		for (auto J = commandLineSetting.second.begin(); J != commandLineSetting.second.end(); ++J) {
			logger->info("Setting command line config option {}:{} to {}", commandLineSetting.first, J->first, J->second);
			mCommandLineConfig.setItem(commandLineSetting.first, J->first, J->second);
			EventChangedConfigItem(commandLineSetting.first, J->first);
		}
	}
	return success;
}

bool ConfigService::saveConfig(const std::filesystem::path& filename) {
	//Go through all user config values and save those (as they were defined in the original user config file).
	//Also save any instance values that aren't present in the user config if they differ from the global value.
	varconf::Config exportConfig;

	//First get the instance values (i.e. those values which have been changed at runtime).
	//But only get those that differs from the global config.
	const varconf::conf_map& instanceSections = mInstanceConfig.getSections();
	for (const auto& instanceSection: instanceSections) {
		const varconf::sec_map& section = instanceSection.second;
		for (const auto& J: section) {
			//only set the value if it differs from the global one
			if (mGlobalConfig.getItem(instanceSection.first, J.first) != J.second) {
				exportConfig.setItem(instanceSection.first, J.first, J.second, varconf::INSTANCE);
			}
		}
	}

	//Then also add all user settings, i.e. those that already had been set in the user config file.
	const varconf::conf_map& userSections = mUserConfig.getSections();
	for (const auto& userSection: userSections) {
		const varconf::sec_map& section = userSection.second;
		for (const auto& J: section) {
			//We can't directly use the value, as it might have been updated in the instance config. We therefore needs to go through getValue
			exportConfig.setItem(userSection.first, J.first, getValue(userSection.first, J.first), varconf::INSTANCE);
		}
	}

	return exportConfig.writeToFile(filename.string());
}


void ConfigService::updatedConfig(const std::string& section, const std::string& key) {
	EventChangedConfigItem.emit(section, key);
}

void ConfigService::configError(const char* error) {
	logger->error(error);
}

std::filesystem::path ConfigService::getHomeDirectory(BaseDirType baseDirType) const {
	//check if the home directory is set, and if so use the setting. If else, fall back to the default path.
	if (!mHomeDir.empty()) {
		return mHomeDir;
	} else {
#ifdef _WIN32
		std::filesystem::path finalPath;

		//special folders in windows:
		//http://msdn.microsoft.com/en-us/library/bb762494%28v=vs.85%29.aspx
		//CSIDL_MYDOCUMENTS would probably be more appropriate, but that's not available in msys (as of 2011-05-23).
		char path[MAX_PATH];
		if(SHGetSpecialFolderPath(nullptr, path, CSIDL_PERSONAL, TRUE)){
			finalPath = path;
		}else{
			finalPath = ".";
		}
		finalPath /= "\\Ember\\";
		return finalPath;
#elif defined(__APPLE__)
		return std::filesystem::path(getAppSupportDirPath()) / "Ember";
#else
		std::filesystem::path path;

		//Determine the directory type requested and ensure that it exists
		switch (baseDirType) {
			case BaseDirType_DATA: {
				path = std::filesystem::path(xdgDataHome(&mBaseDirHandle)) / "ember";
				break;
			}
			case BaseDirType_CONFIG:
				path = std::filesystem::path(xdgConfigHome(&mBaseDirHandle)) / "ember";
				break;
			case BaseDirType_CACHE:
				path = std::filesystem::path(xdgCacheHome(&mBaseDirHandle)) / "ember";
				break;
			case BaseDirType_RUNTIME:
				path = std::filesystem::path(xdgRuntimeDirectory(&mBaseDirHandle)) / "ember";
				break;
		}

		if (!std::filesystem::exists(path)) {
			std::filesystem::create_directories(path);
		}

		return path;
#endif
	}
}

std::filesystem::path ConfigService::getSharedDataDirectory() const {
	if (hasItem("paths", "sharedir")) {
		std::string path = static_cast<std::string> ( getValue("paths", "sharedir")) + "/";
		return path;
	}
#ifdef __APPLE__
	return getBundleResourceDirPath();
#else
	return mSharedDataDir;
#endif

}

std::filesystem::path ConfigService::getSharedConfigDirectory() const {
#ifdef __APPLE__
	return getSharedDataDirectory() / "etc/ember/";
#else
	return mEtcDir;
#endif
}

std::filesystem::path ConfigService::getEmberDataDirectory() const {
	if (hasItem("paths", "datadir")) {
		return {static_cast<std::string> ( getValue("paths", "datadir"))};
	}
//#ifdef __APPLE__
//			return getBundleResourceDirPath();
//#elif __WIN32__
//			return baseDir;
//#else
//			return BR_EMBER_DATADIR("/games/ember/");
//#endif
	return getHomeDirectory(BaseDirType_DATA);
}


std::filesystem::path ConfigService::getUserMediaDirectory() const {
	return getHomeDirectory(BaseDirType_DATA) / "user-media";
}

std::filesystem::path ConfigService::getPluginDirectory() const {
	return mPluginDir;
}


} // namespace Ember
