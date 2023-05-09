import os

from conan import ConanFile, tools
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.files import copy, collect_libs, get, replace_in_file, patch
from conan.tools.system.package_manager import Apt


class OgreConan(ConanFile):
    name = 'ogre'
    upstream_version = "13.4.2"
    package_revision = ""
    version = "{0}{1}".format(upstream_version, package_revision)

    settings = 'os', 'arch', 'compiler', 'build_type'
    options = {'shared': [True, False]}
    default_options = {"shared": False}
    url = 'https://www.ogre3d.org'
    license = 'MIT'
    description = ("Object-Oriented Graphics Rendering Engine (OGRE) "
                   "is a scene-oriented, real-time, 3D rendering engine.")
    short_paths = False
    requires = ["bzip2/1.0.8",
                "zlib/1.2.13",
                "freetype/2.12.1",
                "freeimage/3.18.0@worldforge"]
    user = "worldforge"

    def layout(self):
        cmake_layout(self)

    def system_requirements(self):
        Apt(self).install(["libgl1-mesa-dev", "libegl1-mesa-dev", "libxrandr-dev"])

    def source(self):
        get(self, "https://github.com/OGRECave/ogre/archive/v{0}.tar.gz".format(self.upstream_version), strip_root=True)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['OGRE_BUILD_COMPONENT_PYTHON'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_JAVA'] = 'OFF'
        tc.variables['OGRE_BUILD_SAMPLES'] = 'OFF'
        tc.variables['OGRE_INSTALL_SAMPLES'] = 'OFF'
        tc.variables['OGRE_INSTALL_SAMPLES_SOURCE'] = 'OFF'
        tc.variables['OGRE_BUILD_TESTS'] = 'OFF'
        tc.variables['OGRE_BUILD_TOOLS'] = 'OFF'
        tc.variables['OGRE_BUILD_RENDERSYSTEM_GL'] = 'OFF'
        tc.variables['OGRE_BUILD_RENDERSYSTEM_GLES2'] = 'OFF'
        tc.variables['OGRE_BUILD_RENDERSYSTEM_GL3PLUS'] = "ON"
        tc.variables['OGRE_BUILD_RENDERSYSTEM_D3D9'] = 'OFF'
        tc.variables['OGRE_BUILD_RENDERSYSTEM_D3D11'] = 'OFF'
        tc.variables['OGRE_INSTALL_DOCS'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_BSP'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_CG'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_FREEIMAGE'] = 'ON'
        tc.variables['OGRE_BUILD_PLUGIN_EXRCODEC'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_OCTREE'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_PCZ'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_PFX'] = 'ON'
        tc.variables['OGRE_BUILD_PLUGIN_STBI'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_DOT_SCENE'] = 'OFF'
        tc.variables['OGRE_BUILD_DEPENDENCIES'] = 'OFF'
        tc.variables['OGRE_CONFIG_ENABLE_ZIP'] = "OFF"
        tc.variables['OGRE_BUILD_PLATFORM_APPLE_IOS'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_CSHARP'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_HLMS'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_PAGING'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_TERRAIN'] = 'ON'
        tc.variables['OGRE_BUILD_COMPONENT_RTSHADERSYSTEM'] = 'ON'
        tc.variables['OGRE_BUILD_COMPONENT_VOLUME'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_BITES'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_BULLET'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_PYTHON'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_PROPERTY'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_BITES'] = 'OFF'
        tc.variables['OGRE_BUILD_COMPONENT_OVERLAY'] = 'ON'
        tc.variables['OGRE_BUILD_COMPONENT_OVERLAY_IMGUI'] = 'OFF'
        tc.variables['OGRE_CONFIG_THREAD_PROVIDER'] = 'std'
        tc.variables['OGRE_BUILD_LIBS_AS_FRAMEWORKS'] = 'OFF'
        tc.variables['OGRE_RESOURCEMANAGER_STRICT'] = 'true'
        tc.variables['OGRE_NODELESS_POSITIONING'] = 'OFF'
        tc.variables['OGRE_CONFIG_THREADS'] = '2'
        tc.variables['CMAKE_TOOLCHAIN_FILE'] = 'conan_paths.cmake'
        tc.variables['OGRE_STATIC'] = not self.options.shared

        if self.settings.os == "Windows":
            tc.variables['OGRE_INSTALL_PDB'] = 'OFF'
        else:
            tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        # If we don't disable precompiled headers on Macos we get an error about "Objective-C was disabled in PCH file but is currently enabled"
        if self.settings.os == "Macos":
            tc.variables['OGRE_ENABLE_PRECOMPILED_HEADERS'] = 'OFF'

        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        if self.settings.compiler == "msvc":
            self.cpp_info.libdirs = ["lib", "lib/OGRE", "bin"]
        else:
            self.cpp_info.libdirs = ["lib", "lib/OGRE"]

        self.cpp_info.libs = ["Codec_FreeImageStatic",
                              "OgreMeshLodGeneratorStatic",
                              "OgreOverlayStatic",
                              "OgreTerrainStatic",
                              "OgreRTShaderSystemStatic",
                              "Plugin_ParticleFXStatic",
                              "RenderSystem_GL3PlusStatic",
                              "OgreGLSupportStatic",
                              "OgreMainStatic"]
        if self.settings.os == "Windows":
            self.cpp_info.system_libs = ["Opengl32"]
        elif self.settings.os == "Linux":
            self.cpp_info.system_libs = ["GL", "X11", "Xrandr"]
        elif self.settings.os == "Macos":
            self.cpp_info.system_libs = []
            self.cpp_info.frameworks = ["IOKit", "Cocoa", "Carbon", "OpenGL", "CoreVideo"]

        self.cpp_info.includedirs = ["include/OGRE",
                                     "include/OGRE/Overlay",
                                     "include/OGRE/Terrain",
                                     "include/OGRE/MeshLodGenerator",
                                     "include/OGRE/RTShaderSystem"]
