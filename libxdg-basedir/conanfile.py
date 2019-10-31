from conans import ConanFile, tools, AutoToolsBuildEnvironment
import os
from pathlib import Path

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
        if self.settings.compiler == "Visual Studio":
            #Just skip on win32
            return
        self.run("./autogen.sh")
        autotools = AutoToolsBuildEnvironment(self)
        autotools.configure()
        autotools.make()
        autotools.install()
        #If we're building static libraries we don't want the shared around.
        if not self.options.shared:
            for filename in Path(self.package_folder).glob('**/*.so*'):
                print("Removing shared object file at {}".format(filename))
                os.remove(str(filename))

    def package_info(self):
        self.cpp_info.libs = ['xdg-basedir']
