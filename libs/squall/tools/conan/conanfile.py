import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, update_conandata, collect_libs
from conan.tools.scm import Git
from conans.errors import ConanException


class SquallConan(ConanFile):
    name = "squall"
    version = "0.1.0"
    license = "MIT"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://github.com/worldforge/worldforge/tree/master/libs/squall"
    homepage = "https://www.worldforge.org"
    description = "Distribute and update content to clients."
    topics = "worldforge"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    package_type = "library"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("libcurl/8.10.1")
        self.requires("spdlog/1.14.1")
        self.requires("cli11/2.4.2")
        self.test_requires("catch2/3.7.1")

    def export_sources(self):
        folder = os.path.join(self.recipe_folder, "../..")
        copy(self, "*", folder, self.export_sources_folder, excludes=["build"])
        # We need the "bytesize" external component, so we need to copy that too. We've then also made extra adjustments
        # in CMake to recognize this.
        copy(self, "*", os.path.join(self.recipe_folder, "../../../../external/bytesize"),
             os.path.join(self.export_sources_folder, "external/bytesize"))

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        self.folders.root = "../.."
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = collect_libs(self)
