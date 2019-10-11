# -*- coding: utf-8 -*-

from conans import CMake, ConanFile, tools
from conans.tools import get_env
import os
import platform
import tempfile


class Bullet3Conan(ConanFile):
    name = "bullet3"
    version = "2.88"
    description = "Bullet Physics SDK: real-time collision detection and multi-physics simulation for VR, games, visual effects, robotics, machine learning etc."
    homepage = "https://github.com/bulletphysics/bullet3"
    topics = ("conan", "bullet", "physics", "simulation", "robotics", "kinematics", "engine")
    license = "ZLIB"
    url = ""
    author = ""
    exports_sources = ("CMakeLists.txt", "LICENSE.md", )
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "bullet3": [True, False],
        "graphical_benchmark": [True, False],
        "double_precision": [True, False],
        "bt2_thread_locks": [True, False],
        "btSoftMultiBodyDynamicsWorld": [True, False],
        "network_support": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "bullet3": False,
        "graphical_benchmark": False,
        "double_precision": False,
        "bt2_thread_locks": False,
        "btSoftMultiBodyDynamicsWorld": False,
        "network_support": False,
    }
    no_copy_source = True

    _source_subfolder = "sources"

    def config_options(self):
        if self.options.shared or self.settings.os == "Windows":
            self.options.remove("fPIC")

    def source(self):
        filename = "{}-{}.tar.gz".format(self.name, self.version)
        url = "https://github.com/bulletphysics/bullet3/archive/{}.tar.gz".format(self.version)
        sha256 = "21c135775527754fc2929db1db5144e92ad0218ae72840a9f162acb467a7bbf9"
        extracted_dir = "{}-{}".format(self.name, self.version)

        dlfilepath = os.path.join(tempfile.gettempdir(), filename)
        if os.path.exists(dlfilepath) and not get_env("BULLET3_FORCE_DOWNLOAD", False):
            self.output.info("Skipping download. Using cached {}".format(dlfilepath))
        else:
            self.output.info("Downloading {} from {}".format(self.name, url))
            tools.download(url, dlfilepath)
        tools.check_sha256(dlfilepath, sha256)
        tools.untargz(dlfilepath)

        os.rename(extracted_dir, self._source_subfolder)

    def _configure_cmake(self, cmake):
        cmake.definitions["BUILD_BULLET3"] = self.options.bullet3
        if platform.system() == "Windows":
            cmake.definitions["CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS"] = True
        cmake.definitions["INSTALL_LIBS"] = True
        cmake.definitions["BUILD_SHARED_LIBS"] = self.options.shared
        cmake.definitions["USE_GRAPHICAL_BENCHMARK"] = self.options.graphical_benchmark
        cmake.definitions["USE_DOUBLE_PRECISION"] = self.options.double_precision
        cmake.definitions["BULLET2_USE_THREAD_LOCKS"] = self.options.bt2_thread_locks
        cmake.definitions["USE_SOFT_BODY_MULTI_BODY_DYNAMICS_WORLD"] = self.options.btSoftMultiBodyDynamicsWorld
        cmake.definitions["BUILD_PYBULLET"] = False
        cmake.definitions["BUILD_PYBULLET_NUMPY"] = False
        cmake.definitions["BUILD_ENET"] = self.options.network_support
        cmake.definitions["BUILD_CLSOCKET"] = self.options.network_support
        cmake.definitions["BUILD_CPU_DEMOS"] = False
        cmake.definitions["BUILD_OPENGL3_DEMOS"] = False
        cmake.definitions["BUILD_BULLET2_DEMOS"] = False
        cmake.definitions["BUILD_EXTRAS"] = False
        cmake.definitions["BUILD_UNIT_TESTS"] = False
        if self.settings.compiler == "Visual Studio":
            cmake.definitions["USE_MSVC_RUNTIME_LIBRARY_DLL"] = "MD" in self.settings.compiler.runtime
        cmake.configure(source_folder=self._source_subfolder, build_folder=self.build_folder)
        return cmake

    def build(self):
        tools.replace_in_file("{0}/CMakeLists.txt".format(str(os.path.join(self.source_folder, self._source_subfolder))), "PROJECT(BULLET_PHYSICS)", """PROJECT(BULLET_PHYSICS)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")   
        
        
        cmake = CMake(self)
        self._configure_cmake(cmake)
        with tools.chdir(self.build_folder):
            cmake.build()

    def package(self):
        cmake = CMake(self)
        self._configure_cmake(cmake)
        with tools.chdir(self.build_folder):
            cmake.install()
        self.copy("LICENSE.txt", src=os.path.join(self.source_folder, self._source_subfolder), dst="licenses")

    def package_info(self):
        libs = []
        if self.options.bullet3:
            libs += [
                "Bullet2FileLoader",
                "Bullet3Collision",
                "Bullet3Dynamics",
                "Bullet3Geometry",
                "Bullet3OpenCL_clew",
            ]
        libs += [
            "BulletDynamics",
            "BulletCollision",
            "LinearMath",
            "BulletSoftBody",
            "Bullet3Common",
            "BulletInverseDynamics",
        ]
        if self.settings.os == "Windows" and self.settings.build_type == "Debug":
            libs = [lib + "_Debug" for lib in libs]

        self.cpp_info.libs = libs
        self.cpp_info.includedirs = ["include", "include/bullet"]
        self.cpp_info.builddirs = ["lib/cmake/bullet"]
