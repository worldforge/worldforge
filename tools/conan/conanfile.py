from conans import ConanFile, CMake, tools


class SquallConan(ConanFile):
    name = "squall"
    version = "0.1"
    license = "MIT"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    url = "https://github.com/worldforge/squall"
    homepage = "https://www.worldforge.org"
    description = "Distribute and update content to clients."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake_find_package", "cmake_paths"
    requires = "libcurl/7.87.0", "spdlog/1.11.0", "cli11/2.3.2"
    build_requires = "catch2/3.3.1"

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/squall.git",
        "revision": "auto"
    }

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
