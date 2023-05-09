import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain
from conan.tools.files import copy, update_conandata, collect_libs
from conan.tools.scm import Git
from conans.errors import ConanException


class MercatorConan(ConanFile):
    name = "mercator"
    version = "0.4.0"
    license = "GPL-2.0+"
    homepage = "https://www.worldforge.org"
    url = "https://github.com/worldforge/mercator"
    description = "A terrain generation library for the Worldforge system."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    user = "worldforge"
    generators = "CMakeToolchain"

    def requirements(self):
        self.requires("wfmath/1.0.3@worldforge")

    def export(self):
        git = Git(self, self.recipe_folder)
        try:
            scm_url, scm_commit = git.get_url_and_commit()
            update_conandata(self, {"sources": {"commit": scm_commit, "url": scm_url}})
        except ConanException:
            pass

    def export_sources(self):
        folder = os.path.join(self.recipe_folder, "../..")
        copy(self, "*", folder, self.export_sources_folder, excludes=["*build*"])

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
