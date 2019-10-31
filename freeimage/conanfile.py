#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools
import os
import shutil
from os import path


class FreeImageConan(ConanFile):
    name = "freeimage"
    version = "3.18.0"
    url = ""
    description = "Open source image loading library"
    
    # Indicates License type of the packaged library
    license = "FIPL(http://freeimage.sourceforge.net/freeimage-license.txt)", "GPLv2", "GPLv3"
    
    # Packages the license for the conanfile.py
    exports = ["LICENSE.md"]
    
    # Remove following lines if the target lib does not use cmake.
    exports_sources = ["CMakeLists.txt", "patches/*"]
    generators = "cmake" 
    
    # Options may need to change depending on the packaged library. 
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared"          : [True, False],
        "use_cxx_wrapper" : [True, False],

        # if set, build library without "version number" (eg.: not generate libfreeimage-3-17.0.so)
        "no_soname"       : [True, False]
    }
    default_options = {
        "shared": False,
        "use_cxx_wrapper": True,
        "no_soname": False
    }

    # Custom attributes for Bincrafters recipe conventions
    source_subfolder = "source_subfolder"
    build_subfolder = "build_subfolder"

    # Use version ranges for dependencies unless there's a reason not to
    requires = (
        "zlib/1.2.11")

    short_paths = True


    def configure(self):
        if self.settings.os == "Android":
            self.options.no_soname = True

        self.options.use_cxx_wrapper = False


    def source(self):
        source_url = "http://downloads.sourceforge.net/project/freeimage"
        tools.get(
            "{0}/Source%20Distribution/{1}/FreeImage{2}.zip".format(source_url, self.version, self.version.replace('.', '')),
            sha256='f41379682f9ada94ea7b34fe86bf9ee00935a3147be41b6569c9605a53e438fd')

        #Rename to "source_subfolder" is a convention to simplify later steps
        os.rename("FreeImage", self.source_subfolder)


    def build(self):
        self.apply_patches()
        cmake = CMake(self)
        cmake.configure(build_folder=self.build_subfolder, source_folder=self.source_subfolder)
        cmake.build()

    def package(self):
        self.copy(pattern="LICENSE")

        include_dir = path.join(self.source_subfolder, 'Source')
        self.copy("FreeImage.h", dst="include", src=include_dir)
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.so.*", dst="lib", keep_path=False)
        self.copy("*.dylib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)


    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if not self.options.shared:
            self.cpp_info.defines.append("FREEIMAGE_LIB")


    ################################ Helpers ######################################

    def apply_patches(self):
        self.output.info("Applying patches")

        shutil.copy('CMakeLists.txt', self.source_subfolder)

        if self.settings.compiler == "Visual Studio":
            self.patch_visual_studio()

    def patch_visual_studio(self):
        # snprintf was added in VS2015
        if int(self.settings.compiler.version.value) >= 14:
            tools.replace_in_file(path.join(self.source_subfolder, 'Source/LibRawLite/internal/defines.h'), '#define snprintf _snprintf', '')
            #tools.replace_in_file(path.join(self.source_subfolder, 'Source/LibTIFF4/tif_config.h'), '#define snprintf _snprintf', '')
