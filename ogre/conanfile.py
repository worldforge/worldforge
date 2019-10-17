import os

from conans import ConanFile, CMake, tools
from conans.tools import os_info, SystemPackageTool


class OgreConan(ConanFile):
    name = 'ogre'
    upstream_version = "1.12.3"
    commit_id = "c08a91e70a9bbd3fe257faccf27e1c7eac388946"
    package_revision = ""
    version = "{0}{1}".format(upstream_version, package_revision)

    generators = ['cmake_paths', 'cmake']
    settings = 'os', 'arch', 'compiler', 'build_type'
    options = {'shared': [True, False]}
    default_options = {"shared": False}
    url = 'https://www.ogre3d.org'
    license = 'MIT'
    description = ("Object-Oriented Graphics Rendering Engine (OGRE) "
                   "is a scene-oriented, real-time, 3D rendering engine.")
    source_subfolder = 'source_subfolder'
    short_paths = False
    requires = ["zlib/1.2.11",
                "bzip2/1.0.8",
                "freetype/2.10.0",
                "freeimage/3.18.0@worldforge/stable"]

    def build_requirements(self):
        if os_info.is_linux:
            if os_info.with_apt:
                installer = SystemPackageTool()
                installer.install("libgl1-mesa-dev")
                installer.install("libxrandr-dev")
                installer.install("libxaw7-dev")

    def system_requirements(self):
        if os_info.is_linux:
            if os_info.with_apt:
                installer = SystemPackageTool()
                installer.install("libxrandr")
                installer.install("libxaw7")

    def configure(self):
        if 'CI' not in os.environ:
            os.environ['CONAN_SYSREQUIRES_MODE'] = 'verify'

    def source(self):
        tools.get("https://github.com/OGRECave/ogre/archive/{0}.tar.gz".format(self.commit_id))
        os.rename("ogre-{0}".format(self.commit_id), self.source_subfolder)

    def build(self):
        
        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "include(OgreConfigTargets)", """include(OgreConfigTargets)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")
        cmake = CMake(self)

        cmake.definitions['OGRE_BUILD_COMPONENT_PYTHON'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_JAVA'] = 'OFF'
        cmake.definitions['OGRE_BUILD_SAMPLES'] = 'OFF'
        cmake.definitions['OGRE_INSTALL_SAMPLES'] = 'OFF'
        cmake.definitions['OGRE_INSTALL_SAMPLES_SOURCE'] = 'OFF'
        cmake.definitions['OGRE_BUILD_TESTS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_TOOLS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_GL'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_GLES2'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_GL3PLUS'] = "ON"
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_D3D9'] = 'OFF'
        cmake.definitions['OGRE_BUILD_RENDERSYSTEM_D3D11'] = 'OFF'
        cmake.definitions['OGRE_INSTALL_DOCS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_BSP'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_CG'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_FREEIMAGE'] = 'ON'
        cmake.definitions['OGRE_BUILD_PLUGIN_EXRCODEC'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_OCTREE'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_PCZ'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_PFX'] = 'ON'
        cmake.definitions['OGRE_BUILD_PLUGIN_STBI'] = 'OFF'
        cmake.definitions['OGRE_BUILD_PLUGIN_DOT_SCENE'] = 'OFF'
        cmake.definitions['OGRE_BUILD_DEPENDENCIES'] = 'OFF'
        cmake.definitions['OGRE_INSTALL_DEPENDENCIES'] = 'OFF'
        cmake.definitions['OGRE_CONFIG_ENABLE_ZIP'] = "OFF"
        cmake.definitions['OGRE_BUILD_PLATFORM_APPLE_IOS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_HLMS'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_TERRAIN'] = 'ON'
        cmake.definitions['OGRE_BUILD_COMPONENT_RTSHADERSYSTEM'] = 'ON'
        cmake.definitions['OGRE_BUILD_COMPONENT_VOLUME'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_BITES'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_PYTHON'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_BITES'] = 'OFF'
        cmake.definitions['OGRE_BUILD_COMPONENT_OVERLAY'] = 'ON'
        cmake.definitions['OGRE_CONFIG_THREAD_PROVIDER'] = 'std'
        cmake.definitions['OGRE_BUILD_LIBS_AS_FRAMEWORKS'] = 'OFF'
        cmake.definitions['OGRE_RESOURCEMANAGER_STRICT'] = 'true'
        cmake.definitions['OGRE_NODE_STORAGE_LEGACY'] = 'false'
        cmake.definitions['OGRE_CONFIG_THREADS'] = '2'
        cmake.definitions['CMAKE_TOOLCHAIN_FILE'] = 'conan_paths.cmake'
        cmake.definitions['OGRE_STATIC'] = not self.options.shared

        if tools.os_info.is_windows:
            cmake.definitions['OGRE_INSTALL_PDB'] = 'OFF'
        else:
            cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        cmake.configure(source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = ["include/OGRE"]

    def package(self):
        pass
