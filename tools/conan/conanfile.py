import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy, update_conandata, collect_libs
from conan.tools.scm import Git
from conans.errors import ConanException


class ErisConan(ConanFile):
    name = "eris"
    version = "1.4.0"
    license = "GPL-2.0+"
    homepage = "https://www.worldforge.org"
    url = "https://github.com/worldforge/eris"
    description = "A WorldForge client library."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    user = "worldforge"
    generators = "CMakeDeps", "CMakeToolchain"
    package_type = "library"

    def requirements(self):
        self.requires("sigc++/2.10.0@worldforge", transitive_headers=True)
        self.requires("atlas/0.7.0@worldforge", transitive_headers=True)
        self.requires("wfmath/1.0.3@worldforge", transitive_headers=True)
        self.requires("boost/1.81.0", transitive_headers=True)

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
