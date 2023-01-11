from conans import ConanFile, CMake, tools


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
    generators = "cmake"
    requires = ["sigc++/2.10.0@worldforge/stable",
                "atlas/0.7.0@worldforge/testing",
                "wfmath/1.0.3@worldforge/testing",
                "boost/1.81.0"]

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/eris.git",
        "revision": "auto"
    }

    def imports(self):
        self.copy("*.dll", "bin", "bin")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = ["include/eris-1.4"]

    def package(self):
        pass
