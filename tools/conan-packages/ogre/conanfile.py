import fnmatch
import os

from conan import ConanFile
from conan.tools.apple import is_apple_os
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.files import get, patch
from conan.tools.microsoft import is_msvc
from conan.tools.system.package_manager import Apt


class OgreConan(ConanFile):
    name = 'ogre'
    upstream_version = "14.2.0"
    package_revision = ""
    version = "{0}{1}".format(upstream_version, package_revision)

    settings = 'os', 'arch', 'compiler', 'build_type'
    options = {'shared': [True, False]}
    default_options = {
        "shared": False,
        "freetype/*:with_brotli": False,
        "freetype/*:with_bzip2": False,
        "freetype/*:with_png": False
    }

    url = 'https://www.ogre3d.org'
    license = 'MIT'
    description = ("Object-Oriented Graphics Rendering Engine (OGRE) "
                   "is a scene-oriented, real-time, 3D rendering engine.")
    short_paths = False
    requires = [
        "freetype/2.13.2",
        "glslang/11.7.0"
    ]
    user = "worldforge"
    package_type = "library"
    exports_sources = ["patches*"]

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
        tc.variables['OGRE_BUILD_PLUGIN_FREEIMAGE'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_EXRCODEC'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_OCTREE'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_PCZ'] = 'OFF'
        tc.variables['OGRE_BUILD_PLUGIN_PFX'] = 'ON'
        tc.variables['OGRE_BUILD_PLUGIN_STBI'] = 'ON'
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
        tc.variables['OGRE_BUILD_COMPONENT_OVERLAY'] = 'ON'
        tc.variables['OGRE_BUILD_COMPONENT_OVERLAY_IMGUI'] = 'OFF'
        tc.variables['OGRE_CONFIG_THREAD_PROVIDER'] = 'std'
        tc.variables['OGRE_BUILD_LIBS_AS_FRAMEWORKS'] = 'OFF'
        tc.variables['OGRE_RESOURCEMANAGER_STRICT'] = 'true'
        tc.variables['OGRE_NODELESS_POSITIONING'] = 'OFF'
        tc.variables['OGRE_CONFIG_THREADS'] = '3'
        tc.variables['OGRE_STATIC'] = not self.options.shared

        if is_msvc(self):
            tc.variables['OGRE_INSTALL_PDB'] = 'OFF'
        else:
            tc.variables['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        # If we don't disable precompiled headers on Macos we get an error about "Objective-C was disabled in PCH file but is currently enabled"
        if is_apple_os(self):
            tc.variables['OGRE_ENABLE_PRECOMPILED_HEADERS'] = 'OFF'

        tc.generate()

    def build(self):
        self._apply_patches(os.path.join(self.export_sources_folder, 'patches'), self.folders.source_folder)
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        if is_msvc(self):
            self.cpp_info.libdirs = ["lib", "lib/OGRE", "bin"]
        else:
            self.cpp_info.libdirs = ["lib", "lib/OGRE"]

        self.cpp_info.libs = ["Codec_STBIStatic",
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

        if is_apple_os(self) or is_msvc(self):
            self.cpp_info.builddirs.append("CMake")
        else:
            self.cpp_info.builddirs.append(os.path.join("lib", "OGRE", "cmake"))

    def _apply_patches(self, source, dest):
        for root, _dirnames, filenames in os.walk(source):
            for filename in fnmatch.filter(filenames, '*.patch'):
                patch_file = os.path.join(root, filename)
                print("Applying patch {}.".format(patch_file))
                dest_path = os.path.join(dest, os.path.relpath(root, source))
                patch(self, base_path=dest_path, patch_file=patch_file)