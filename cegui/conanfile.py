import fnmatch
import os

from conans import ConanFile, CMake, tools


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
    exports_sources = ["CMakeLists.txt", "patches*"]
    generators = ["cmake", "cmake_paths"]

    # Options may need to change depending on the packaged library.
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
    }
    default_options = {
        "shared": False,
        "pcre:with_unicode_properties": True,
        "pcre:with_bzip2": False
    }

    # Custom attributes for Bincrafters recipe conventions
    source_subfolder = "source_subfolder"

    requires = (
        "freetype/2.11.1",
        "freeimage/3.18.0@worldforge/stable",
        "tinyxml/2.6.2",
        "pcre/8.45",
        "libpng/1.6.37",
        "bzip2/1.0.8",
        "libiconv/1.16",
        "zlib/1.2.12"
    )

    short_paths = True

    def source(self):
        version_hyphenated = self.version.replace(".", "-")
        extracted_dir = "cegui-" + version_hyphenated
        source_url = "https://github.com/cegui/cegui/archive"
        tools.get("{0}/v{1}.zip".format(source_url, version_hyphenated))

        # Rename to "source_subfolder" is a convention to simplify later steps
        os.rename(extracted_dir, self.source_subfolder)

        self._apply_patches('patches', self.source_subfolder)

    def build(self):
        # Remove VS snprintf workaround
        if self.settings.compiler == 'Visual Studio' and int(str(self.settings.compiler.version)) >= 14:
            tools.replace_in_file('{0}/cegui/include/CEGUI/PropertyHelper.h'.format(self.source_subfolder), '#define snprintf _snprintf', '')
                
        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "project(cegui)", """project(cegui)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")        

        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = True
        cmake.definitions['CEGUI_SAMPLES_ENABLED'] = False
        cmake.definitions['CEGUI_BUILD_PYTHON_MODULES'] = False
        cmake.definitions['CEGUI_BUILD_LUA_MODULE'] = True
        cmake.definitions['CEGUI_BUILD_APPLICATION_TEMPLATES'] = False
        cmake.definitions['CEGUI_HAS_FREETYPE'] = True
        cmake.definitions['CEGUI_OPTION_DEFAULT_IMAGECODEC'] = 'FreeImageImageCodec'
        cmake.definitions['CEGUI_BUILD_IMAGECODEC_FREEIMAGE'] = 'ON'
        cmake.definitions['CEGUI_BUILD_IMAGECODEC_SDL2'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_OGRE'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_OPENGL'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_OPENGL3'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_OPENGLES'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_DIRECT3D9'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_DIRECT3D10'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_DIRECT3D11'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_RENDERER_DIRECTFB'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_LUA_GENERATOR'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_XMLPARSER_EXPAT'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_XMLPARSER_LIBXML2'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_XMLPARSER_XERCES'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_XMLPARSER_RAPIDXML'] = 'OFF'
        cmake.definitions['CEGUI_BUILD_XMLPARSER_TINYXML'] = 'ON'
        cmake.definitions['CEGUI_BUILD_XMLPARSER_TINYXML2'] = 'OFF'
        cmake.definitions['CEGUI_OPTION_DEFAULT_XMLPARSER'] = 'TinyXMLParser'
        cmake.definitions['CMAKE_TOOLCHAIN_FILE'] = 'conan_paths.cmake'
        if not self.options.shared:
            cmake.definitions['CEGUI_BUILD_STATIC_CONFIGURATION'] = 'ON'
            cmake.definitions['CEGUI_BUILD_STATIC_FACTORY_MODULE'] = 'ON'
            cmake.definitions['CEGUI_BUILD_SHARED_LIBS_WITH_STATIC_DEPENDENCIES'] = 'ON'

        cmake.configure(source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package(self):
        pass

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if not self.options.shared:
            self.cpp_info.libs = list(filter(lambda lib: str(lib).endswith("_Static"), self.cpp_info.libs))
            self.cpp_info.defines = ["CEGUI_STATIC"]
        self.cpp_info.includedirs = ["include/cegui-0"]

    @staticmethod
    def _apply_patches(source, dest):
        for root, _dirnames, filenames in os.walk(source):
            for filename in fnmatch.filter(filenames, '*.patch'):
                print("Applying path {}.".format(filename))
                patch_file = os.path.join(root, filename)
                dest_path = os.path.join(dest, os.path.relpath(root, source))
                tools.patch(base_path=dest_path, patch_file=patch_file)
