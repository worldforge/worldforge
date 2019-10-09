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
    options = {"shared": [False, True]}
    default_options = {"shared": False}
    requires = ["bzip2/1.0.8@_/_",
                "zlib/1.2.11@_/_"]
    generators = "cmake"

    scm = {
        "type": "git",
        "url": "https://github.com/worldforge/atlas-cpp.git",
        "revision": "auto"
    }

    def build(self):
        cmake = CMake(self)
        cmake.definitions['ATLAS_GENERATE_OBJECTS'] = Fal
        cmake.configure(source_folder=".")
        cmake.build()
        cmake.install()

    def package_info(self):
        # Since the libraries are interdependent we must provide them in correct order.
        self.cpp_info.libs = ["AtlasNet-0.7", "AtlasObjects-0.7", "AtlasFilters-0.7", "AtlasCodecs-0.7", "AtlasMessage-0.7", "Atlas-0.7"]
        self.cpp_info.includedirs = ["include/Atlas-0.7"]

    def package(self):
        pass
