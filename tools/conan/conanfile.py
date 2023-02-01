from conans import ConanFile, CMake, tools


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
    generators = "cmake_find_package", "cmake_paths"
    requires = "wfmath/1.0.3@worldforge/testing"

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/mercator.git",
        "revision": "auto"
    }

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

    def package(self):
        pass
