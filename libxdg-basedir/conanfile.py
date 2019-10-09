from conans import ConanFile, tools, CMake, AutoToolsBuildEnvironment


class Lua(ConanFile):
    name = 'libxdg-basedir'
    version = '1.2.0'
    license = 'MIT'
    url = ''
    homepage = ''
    description = ''
    settings = 'os', 'arch', 'build_type', 'compiler'
    options = {'shared': [True, False]}
    default_options = {"shared": False}

    def source(self):
        git = tools.Git()
        git.clone("https://github.com/devnev/libxdg-basedir.git", "libxdg-basedir-1.2.0")

    def build(self):
        self.run("./autogen.sh")
        autotools = AutoToolsBuildEnvironment(self)
        autotools.configure()
        autotools.make()
        autotools.install()

    # def package(self):
    #     self.copy("tolua++.h", dst="include", src="include")
    #     self.copy('*.lib', dst='lib', keep_path=False)
    #     self.copy('*.dll', dst='lib', keep_path=False)
    #     self.copy('*.so', dst='lib', keep_path=False)
    #     self.copy('*.dylib', dst='lib', keep_path=False)
    #     self.copy('*.a', dst='lib', keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ['xdg-basedir']
