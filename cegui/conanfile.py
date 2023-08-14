import fnmatch
import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.files import replace_in_file, get, collect_libs, patch
from conan.tools.microsoft import is_msvc


class CeguiConan(ConanFile):
    name = "cegui"
    version = "0.8.7"
    url = "http://cegui.org.uk/"
    description = "Crazy Eddie's GUI"

    # Indicates License type of the packaged library
    license = "https://opensource.org/licenses/mit-license.php"

    # Packages the license for the conanfile.py
    exports = ["LICENSE.md"]

    # Remove following lines if the target lib does not use cmake.
    exports_sources = ["patches*"]

    # Options may need to change depending on the packaged library.
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
    }
    default_options = {
        "shared": False,
        "freetype/*:with_brotli": False,
        "freetype/*:with_bzip2": False,
        "freetype/*:with_png": False
    }
    user = "worldforge"
    package_type = "library"

    def requirements(self):
        self.requires("freetype/2.13.0")
        self.requires("pcre/8.45")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = True
        tc.variables['CEGUI_SAMPLES_ENABLED'] = False
        tc.variables['CEGUI_BUILD_PYTHON_MODULES'] = False
        tc.variables['CEGUI_BUILD_LUA_MODULE'] = False
        tc.variables['CEGUI_BUILD_APPLICATION_TEMPLATES'] = False
        tc.variables['CEGUI_HAS_FREETYPE'] = True
        tc.variables['CEGUI_HAS_PCRE_REGEX'] = True
        tc.variables['CEGUI_BUILD_IMAGECODEC_FREEIMAGE'] = 'OFF'
        tc.variables['CEGUI_BUILD_IMAGECODEC_SDL2'] = 'OFF'
        tc.variables['CEGUI_BUILD_IMAGECODEC_STB'] = 'ON'
        tc.variables['CEGUI_BUILD_RENDERER_OGRE'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_OPENGL'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_OPENGL3'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_OPENGLES'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_DIRECT3D9'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_DIRECT3D10'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_DIRECT3D11'] = 'OFF'
        tc.variables['CEGUI_BUILD_RENDERER_DIRECTFB'] = 'OFF'
        tc.variables['CEGUI_BUILD_LUA_GENERATOR'] = 'OFF'
        tc.variables['CEGUI_BUILD_XMLPARSER_EXPAT'] = 'OFF'
        tc.variables['CEGUI_BUILD_XMLPARSER_LIBXML2'] = 'OFF'
        tc.variables['CEGUI_BUILD_XMLPARSER_XERCES'] = 'OFF'
        tc.variables['CEGUI_BUILD_XMLPARSER_RAPIDXML'] = 'OFF'
        tc.variables['CEGUI_BUILD_XMLPARSER_TINYXML'] = 'OFF'
        tc.variables['CEGUI_BUILD_XMLPARSER_TINYXML2'] = 'OFF'
        tc.variables['CEGUI_OPTION_DEFAULT_XMLPARSER'] = 'None'
        if not self.options.shared:
            tc.variables['CEGUI_BUILD_STATIC_CONFIGURATION'] = 'ON'
            tc.variables['CEGUI_BUILD_STATIC_FACTORY_MODULE'] = 'ON'
            tc.variables['CEGUI_BUILD_SHARED_LIBS_WITH_STATIC_DEPENDENCIES'] = 'ON'

        tc.generate()

        CMakeDeps(self).generate()

    def layout(self):
        cmake_layout(self)

    def source(self):
        version_hyphenated = self.version.replace(".", "-")
        source_url = "https://github.com/cegui/cegui/archive"
        get(self, "{0}/v{1}.zip".format(source_url, version_hyphenated), strip_root=True)

    def build(self):
        self._apply_patches(os.path.join(self.export_sources_folder, 'patches'), self.folders.source_folder)
        # Remove VS snprintf workaround
        if is_msvc(self) and int(str(self.settings.compiler.version)) >= 14:
            replace_in_file(self, '{0}/cegui/include/CEGUI/PropertyHelper.h'.format(self.folders.source_folder),
                            '#define snprintf _snprintf', '')
        replace_in_file(self, "{0}/CMakeLists.txt".format(self.folders.source_folder), "project(cegui)", """project(cegui)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)
            """)

        # We shouldn't even look for boost.
        replace_in_file(self, "{0}/CMakeLists.txt".format(self.folders.source_folder), "find_package(Boost",
                        "#find_package(Boost")

        replace_in_file(self, "{0}/CMakeLists.txt".format(self.folders.source_folder), "find_package(Freetype)",
                        "find_package(Freetype CONFIG REQUIRED)\nlink_libraries(Freetype::Freetype)")
        replace_in_file(self, "{0}/CMakeLists.txt".format(self.folders.source_folder), "find_package(PCRE)",
                        "find_package(PCRE CONFIG REQUIRED)\nlink_libraries(pcre::pcre)")
        replace_in_file(self, "{0}/CMakeLists.txt".format(self.folders.source_folder),
                        "find_package(GTK2 COMPONENTS gtk)",
                        "#find_package(GTK2 COMPONENTS gtk)")

        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
        if not self.options.shared:
            self.cpp_info.libs = list(filter(lambda lib: str(lib).endswith("_Static"), self.cpp_info.libs))
            self.cpp_info.defines = ["CEGUI_STATIC"]
        self.cpp_info.includedirs = ["include/cegui-0"]

    def _apply_patches(self, source, dest):
        for root, _dirnames, filenames in os.walk(source):
            for filename in fnmatch.filter(filenames, '*.patch'):
                print("Applying path {}.".format(filename))
                patch_file = os.path.join(root, filename)
                dest_path = os.path.join(dest, os.path.relpath(root, source))
                patch(self, base_path=dest_path, patch_file=patch_file)
