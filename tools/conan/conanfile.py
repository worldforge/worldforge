import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout, CMakeToolchain, CMakeDeps
from conan.tools.files import copy, update_conandata
from conan.tools.scm import Git
from conans.errors import ConanException


class AtlasConan(ConanFile):
    name = "atlas"
    version = "0.7.0"
    license = "GPL-2.0+"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    homepage = "https://www.worldforge.org"
    url = "https://github.com/worldforge/atlas-cpp"
    description = "Networking protocol for the Worldforge system."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    requires = ["bzip2/1.0.8",
                "zlib/1.2.13"]
    user = "worldforge"
    package_type = "library"

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

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables['ATLAS_GENERATE_OBJECTS'] = False
        tc.generate()

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
        self.cpp_info.set_property("cmake_file_name", "Atlas")
        self.cpp_info.set_property("cmake_target_name", "Atlas::Atlas")
        self.cpp_info.names["cmake_find_package"] = "Atlas"
        self.cpp_info.names["cmake_find_package_multi"] = "Atlas"

        self.cpp_info.components["Atlas"].libs = ["Atlas"]
        self.cpp_info.components["Atlas"].set_property("cmake_target_name", "Atlas::Atlas")
        self.cpp_info.components["Atlas"].names["cmake_find_package"] = "Atlas"
        self.cpp_info.components["Atlas"].names["cmake_find_package_multi"] = "Atlas"

        self.cpp_info.components["AtlasCodecs"].libs = ["AtlasCodecs"]
        self.cpp_info.components["AtlasCodecs"].set_property("cmake_target_name", "Atlas::AtlasCodecs")
        self.cpp_info.components["AtlasCodecs"].names["cmake_find_package"] = "AtlasCodecs"
        self.cpp_info.components["AtlasCodecs"].names["cmake_find_package_multi"] = "AtlasCodecs"
        self.cpp_info.components["AtlasCodecs"].requires = ["Atlas"]

        self.cpp_info.components["AtlasMessage"].libs = ["AtlasMessage"]
        self.cpp_info.components["AtlasMessage"].set_property("cmake_target_name", "Atlas::AtlasMessage")
        self.cpp_info.components["AtlasMessage"].names["cmake_find_package"] = "AtlasMessage"
        self.cpp_info.components["AtlasMessage"].names["cmake_find_package_multi"] = "AtlasMessage"
        self.cpp_info.components["AtlasMessage"].requires = ["Atlas"]

        self.cpp_info.components["AtlasNet"].libs = ["AtlasNet"]
        self.cpp_info.components["AtlasNet"].set_property("cmake_target_name", "Atlas::AtlasNet")
        self.cpp_info.components["AtlasNet"].names["cmake_find_package"] = "AtlasNet"
        self.cpp_info.components["AtlasNet"].names["cmake_find_package_multi"] = "AtlasNet"
        self.cpp_info.components["AtlasNet"].requires = ["Atlas", "AtlasCodecs"]

        self.cpp_info.components["AtlasObjects"].libs = ["AtlasObjects"]
        self.cpp_info.components["AtlasObjects"].set_property("cmake_target_name", "Atlas::AtlasObjects")
        self.cpp_info.components["AtlasObjects"].names["cmake_find_package"] = "AtlasObjects"
        self.cpp_info.components["AtlasObjects"].names["cmake_find_package_multi"] = "AtlasObjects"
        self.cpp_info.components["AtlasObjects"].requires = ["Atlas", "AtlasMessage", "AtlasCodecs"]

        self.cpp_info.components["AtlasFilters"].libs = ["AtlasFilters"]
        self.cpp_info.components["AtlasFilters"].set_property("cmake_target_name", "Atlas::AtlasFilters")
        self.cpp_info.components["AtlasFilters"].names["cmake_find_package"] = "AtlasFilters"
        self.cpp_info.components["AtlasFilters"].names["cmake_find_package_multi"] = "AtlasFilters"
        self.cpp_info.components["AtlasFilters"].requires = ["Atlas"]
