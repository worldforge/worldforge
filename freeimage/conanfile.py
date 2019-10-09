from conans import ConanFile, AutoToolsBuildEnvironment, tools
import os
from os import path

class FreeImage(ConanFile):
    name = "freeimage"
    version = "3.18.0"
    license = "FIPL(http://freeimage.sourceforge.net/freeimage-license.txt)", "GPLv2", "GPLv3"
    url = ""
    homepage = ""
    description = "FreeImage is an Open Source library project for developers who would like to support popular graphics image formats like PNG, BMP, JPEG, TIFF and others as needed by today's multimedia applications."
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = "shared=False", "fPIC=False"
    generators = "cmake"
    source_subfolder = "source_subfolder"
    #package_folder = source_subfolder

    def configure(self):
        if self.settings.os == "Windows":
            self.options.remove("fPIC")

    def source(self):
        source_url = "http://downloads.sourceforge.net/project/freeimage"
        tools.get(
            "{0}/Source%20Distribution/{1}/FreeImage{2}.zip".format(source_url, self.version, self.version.replace('.', '')),
            sha256='f41379682f9ada94ea7b34fe86bf9ee00935a3147be41b6569c9605a53e438fd')

        # Rename to "source_subfolder" is a convention to simplify later steps
        os.rename("FreeImage", self.source_subfolder)

    def build(self):
        os.chdir(self.source_subfolder)
        autotools = AutoToolsBuildEnvironment(self)
        env_build_vars = autotools.vars
        env_build_vars['DESTDIR'] = self.package_folder
        autotools.make(vars=env_build_vars)

    def package(self):
        self.copy(pattern="LICENSE")

        include_dir = path.join(self.source_subfolder, 'Dist')
        self.copy("FreeImage.h", dst="include", src=include_dir)
        self.copy("*.lib", dst="lib", keep_path=False, src=include_dir)
        self.copy("*.a", dst="lib", keep_path=False, src=include_dir)
        self.copy("*.so", dst="lib", keep_path=False, src=include_dir)
        self.copy("*.so.*", dst="lib", keep_path=False, src=include_dir)
        self.copy("*.dylib", dst="lib", keep_path=False, src=include_dir)
        self.copy("*.dll", dst="bin", keep_path=False, src=include_dir)

    def package_info(self):
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libdirs = ["lib"]
        self.cpp_info.libs = ["freeimage"]


# import os
# import shutil
# from os import path
#
# from conans import ConanFile, CMake, tools
#
#
# class FreeImageConan(ConanFile):
#     name = "freeimage"
#     version = "3.18.0"
#     url = ""
#     description = "Open source image loading library"
#
#     # Indicates License type of the packaged library
#     license = "FIPL(http://freeimage.sourceforge.net/freeimage-license.txt)", "GPLv2", "GPLv3"
#
#     # Packages the license for the conanfile.py
#     exports = ["LICENSE.md"]
#
#     # Remove following lines if the target lib does not use cmake.
#     exports_sources = ["CMakeLists.txt", "patches/*"]
#     generators = "cmake"
#
#     # Options may need to change depending on the packaged library.
#     settings = "os", "arch", "compiler", "build_type"
#     options = {
#         "shared": [True, False],
#         "use_cxx_wrapper": [True, False],
#
#         # if set, build library without "version number" (eg.: not generate libfreeimage-3-17.0.so)
#         "no_soname": [True, False]
#     }
#     default_options = (
#         "shared=False",
#         "use_cxx_wrapper=True",
#         "no_soname=False"
#     )
#
#     # Custom attributes for Bincrafters recipe conventions
#     source_subfolder = "source_subfolder"
#     build_subfolder = "build_subfolder"
#
#     # Use version ranges for dependencies unless there's a reason not to
#     requires = (
#         "zlib/1.2.11",
#         "libjpeg/9c",
#         "libpng/1.6.37",
#         "libtiff/4.0.9",
#         "libwebp/1.0.3",
#         # "openexr/2.3.0@conan/stable",  # 2.2.1 is used by FreeImage 3.18.0, however conan community doesn't provide it
#     )
#
#     short_paths = True
#
#     def configure(self):
#         if self.settings.os == "Android":
#             self.options.no_soname = True
#
#         # TODO: Implement & enable this for CMake (Wrapper/FreeImagePlus)
#         self.options.use_cxx_wrapper = False
#
#     def source(self):
#         source_url = "http://downloads.sourceforge.net/project/freeimage"
#         tools.get(
#             "{0}/Source%20Distribution/{1}/FreeImage{2}.zip".format(source_url, self.version, self.version.replace('.', '')),
#             sha256='f41379682f9ada94ea7b34fe86bf9ee00935a3147be41b6569c9605a53e438fd')
#
#         # Rename to "source_subfolder" is a convention to simplify later steps
#         os.rename("FreeImage", self.source_subfolder)
#
#     def build(self):
#         self.apply_patches()
#         cmake = CMake(self)
#         cmake.configure(build_folder=self.build_subfolder, source_folder=self.source_subfolder)
#         cmake.build()
#
#     def package(self):
#         self.copy(pattern="LICENSE")
#
#         include_dir = path.join(self.source_subfolder, 'Source')
#         self.copy("FreeImage.h", dst="include", src=include_dir)
#         self.copy("*.lib", dst="lib", keep_path=False)
#         self.copy("*.a", dst="lib", keep_path=False)
#         self.copy("*.so", dst="lib", keep_path=False)
#         self.copy("*.so.*", dst="lib", keep_path=False)
#         self.copy("*.dylib", dst="lib", keep_path=False)
#         self.copy("*.dll", dst="bin", keep_path=False)
#
#     def package_info(self):
#         self.cpp_info.libs = tools.collect_libs(self)
#         if not self.options.shared:
#             self.cpp_info.defines.append("FREEIMAGE_LIB")
#
#     ################################ Helpers ######################################
#
#     def apply_patches(self):
#         self.output.info("Applying patches")
#
#         # Copy "patch" files
#         shutil.copy('CMakeLists.txt', self.source_subfolder)
#         # self.copy_tree("patches", self.source_subfolder)
#
#         # self.patch_gcc7()
#         self.patch_zlib()
#         self.patch_jpeg(['Source/FreeImage/PluginJPEG.cpp', 'Source/FreeImageToolkit/JPEGTransform.cpp'])
#
#         # self.patch_android_swab_issues()
#
#         self.patch_cmake()
#         if self.settings.compiler == "Visual Studio":
#             self.patch_visual_studio()
#
#     # def patch_gcc7(self):
#     #     tools.patch(base_path=self.source_subfolder, patch_file='patches/gcc7.patch')
#
#     def patch_zlib(self):
#         # Remove ZLib directory since we're using conan for zlib
#         # shutil.rmtree(path.join(self.source_subfolder, 'Source/ZLib'))
#         # shutil.rmtree(path.join(self.source_subfolder, 'Source/LibJPEG'))
#         shutil.rmtree(path.join(self.source_subfolder, 'Source/LibPNG'))
#         # shutil.rmtree(path.join(self.source_subfolder, 'Source/LibTIFF4'))
#         shutil.rmtree(path.join(self.source_subfolder, 'Source/LibWebP'))
#         shutil.rmtree(path.join(self.source_subfolder, 'Source/OpenEXR'))
#
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/ZLibInterface.cpp'), '#include "../ZLib/zlib.h"', '#include <zlib.h>')
#         # tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/ZLibInterface.cpp'), '#include "../ZLib/zutil.h"', '#include <zutil.h>')
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginPNG.cpp'), '#include "../ZLib/zlib.h"', '#include <zlib.h>')
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginPNG.cpp'), '#include "../LibPNG/png.h"', '#include <png.h>')
#
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginEXR.cpp'), '../OpenEXR/IlmImf/', '')
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginEXR.cpp'), '../OpenEXR/Iex/', '')
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginEXR.cpp'), '../OpenEXR/Half/', '')
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginTIFF.cpp'), '../OpenEXR/Half/', '')
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/PluginWebP.cpp'), '../LibWebP/src/', '')
#
#     def patch_jpeg(self, files):
#         for f in files:
#             tools.replace_in_file(path.join(self.source_subfolder, f), '#include "../LibJPEG/jpeglib.h"', '#include <jpeglib.h>')
#             tools.replace_in_file(path.join(self.source_subfolder, f), '#include "../LibJPEG/jerror.h"', '#include <jerror.h>')
#
#     # def patch_android_swab_issues(self):
#     #     librawlite = path.join(self.source_subfolder, "Source", "LibRawLite")
#     #     missing_swab_files = [
#     #         path.join(librawlite, "dcraw", "dcraw.c"),
#     #         path.join(librawlite, "internal", "defines.h")
#     #     ]
#     #     replaced_include = '\n'.join(('#include <unistd.h>', '#include "swab.h"'))
#     #
#     #     for f in missing_swab_files:
#     #         self.output.info("patching file '%s'" % f)
#     #         tools.replace_in_file(f, "#include <unistd.h>", replaced_include)
#
#     def patch_cmake(self):
#         tools.replace_in_file(path.join(self.source_subfolder, 'Source/FreeImage/Plugin.cpp'), 's_plugins->AddNode(InitJXR);', '')
#
#     def patch_visual_studio(self):
#         # snprintf was added in VS2015
#         if int(self.settings.compiler.version.value) >= 14:
#             tools.replace_in_file(path.join(self.source_subfolder, 'Source/LibRawLite/internal/defines.h'), '#define snprintf _snprintf', '')
#             # tools.replace_in_file(path.join(self.source_subfolder, 'Source/LibTIFF4/tif_config.h'), '#define snprintf _snprintf', '')
