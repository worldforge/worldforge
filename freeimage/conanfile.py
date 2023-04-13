#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import shutil

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, update_conandata, collect_libs, get, replace_in_file, patch
from conan.tools.scm import Git
from conans.errors import ConanException


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
    generators = "CMakeDeps", "CMakeToolchain"

    # Options may need to change depending on the packaged library. 
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "use_cxx_wrapper": [True, False],

        # if set, build library without "version number" (eg.: not generate libfreeimage-3-17.0.so)
        "no_soname": [True, False]
    }
    default_options = {
        "shared": False,
        "use_cxx_wrapper": True,
        "no_soname": False
    }

    # Use version ranges for dependencies unless there's a reason not to
    requires = (
        "zlib/1.2.13")

    short_paths = True
    user = "worldforge"

    def configure(self):
        if self.settings.os == "Android":
            self.options.no_soname = True

    def source(self):
        source_url = "http://downloads.sourceforge.net/project/freeimage"
        get(self,
            "{0}/Source%20Distribution/{1}/FreeImage{2}.zip".format(source_url, self.version,
                                                                    self.version.replace('.', '')),
            sha256='f41379682f9ada94ea7b34fe86bf9ee00935a3147be41b6569c9605a53e438fd',
            strip_root=True)

    def build(self):
        self.apply_patches()
        cmake = CMake(self)
        cmake.configure()
        cmake.build()


    def layout(self):
        cmake_layout(self)

    def package(self):
        copy(self, pattern="LICENSE", src=self.build_folder, dst=self.package_folder)
        copy(self, "FreeImage.h", dst=os.path.join(self.package_folder, "include"),
             src=os.path.join(self.source_folder, "Source"))
        copy(self, "*.lib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.a", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.so", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.so.*", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.dylib", src=self.build_folder, dst=os.path.join(self.package_folder, "lib"), keep_path=False)
        copy(self, "*.dll", src=self.build_folder, dst=os.path.join(self.package_folder, "bin"), keep_path=False)

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        if not self.options.shared:
            self.cpp_info.defines.append("FREEIMAGE_LIB")

    ################################ Helpers ######################################

    def apply_patches(self):
        # Allow for compilation with C++17
        replace_in_file(self, os.path.join(self.source_folder, "Source/OpenEXR/Imath/ImathVec.h"),
                        " throw (IEX_NAMESPACE::MathExc)", "")
        replace_in_file(self, os.path.join(self.source_folder, "Source/OpenEXR/Imath/ImathVec.cpp"),
                        " throw (IEX_NAMESPACE::MathExc)", "")
        replace_in_file(self, os.path.join(self.source_folder, "Source/OpenEXR/Imath/ImathMatrix.h"),
                        " throw (IEX_NAMESPACE::MathExc)", "")

        if self.settings.os == "Macos":
            patch(self, patch_file='patches/zlib.patch')
            patch(self, patch_file='patches/jxr.patch')

        if self.settings.compiler == "msvc":
            self.patch_visual_studio()

    def patch_visual_studio(self):
        # snprintf was added in VS2015
        if int(self.settings.compiler.version.value) >= 14:
            replace_in_file(self, 'Source/LibRawLite/internal/defines.h',
                            '#define snprintf _snprintf', '')
            # tools.replace_in_file(path.join(self.source_subfolder, 'Source/LibTIFF4/tif_config.h'), '#define snprintf _snprintf', '')
