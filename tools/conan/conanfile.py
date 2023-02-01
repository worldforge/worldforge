from conans import ConanFile, CMake


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
    generators = "cmake_find_package", "cmake_paths"

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/atlas-cpp.git",
        "revision": "auto"
    }

    def imports(self):
        self.copy("*.dll", "bin", "bin")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions['ATLAS_GENERATE_OBJECTS'] = False
        print(cmake.definitions)
        cmake.configure(source_folder=".")
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package_info(self):
        # Since the libraries are interdependent we must provide them in correct order.
        self.cpp_info.libs = ["AtlasNet", "AtlasObjects", "AtlasFilters", "AtlasCodecs", "AtlasMessage", "Atlas"]

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
