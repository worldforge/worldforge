from conans import ConanFile, CMake, tools


class WfmathConan(ConanFile):
    name = "wfmath"
    version = "1.0.3"
    license = "GPL-2.0+"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    homepage = "https://www.worldforge.org"
    url = "https://github.com/worldforge/wfmath"
    description = "A math library for the Worldforge system."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    generators = "cmake_find_package", "cmake_paths"

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/wfmath.git",
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
