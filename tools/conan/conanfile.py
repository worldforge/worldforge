from conans import ConanFile, CMake, tools


class VarconfConan(ConanFile):
    name = "varconf"
    version = "1.0.3"
    license = "GPL-2.0+"
    author = "Erik Ogenvik <erik@ogenvik.org>"
    homepage = "https://www.worldforge.org"
    url = "https://github.com/worldforge/varconf"
    description = "Configuration library for the Worldforge system."
    topics = ("mmorpg", "worldforge")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [False, True]}
    default_options = {"shared": False}
    generators = "cmake"
    requires = ["sigc++/2.10.2@worldforge/stable"]

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/varconf.git",
        "revision": "auto"
    }

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = ["include/varconf-1.0"]

    def package(self):
        pass
