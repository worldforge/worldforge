from conans import ConanFile, CMake, tools


class SquallConan(ConanFile):
    name = "squall"
    version = "0.1"
    license = "MIT"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://github.com/worldforge/squall"
    description = "Distribute and update content to clients."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake"
    requires = "libcurl/7.84.0", "spdlog/1.10.0", "cli11/2.2.0"
    build_requires = "catch2/3.1.0"

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/squall.git",
        "revision": "auto"
    }

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = ["include/squall-0.1"]

    def package(self):
        pass
