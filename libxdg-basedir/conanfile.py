from conan import ConanFile
from conan.tools.gnu import Autotools, AutotoolsToolchain
import os
from pathlib import Path

from conan.tools.layout import basic_layout
from conan.tools.scm import Git


class Pkg(ConanFile):
    name = 'libxdg-basedir'
    version = '1.2.3'
    license = 'MIT'
    url = 'https://github.com/devnev/libxdg-basedir'
    homepage = ''
    description = ''
    settings = 'os', 'arch', 'build_type', 'compiler'
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {"shared": False, "fPIC": True}

    def source(self):
        git = Git(self)
        git.clone(url="https://github.com/devnev/libxdg-basedir.git", target=".")

    def layout(self):
        basic_layout(self)

    def generate(self):
        if self.settings.compiler == "msvc":
            # Just skip on win32
            return
        tc = AutotoolsToolchain(self)
        tc.generate()

    def build(self):
        if self.settings.compiler == "msvc":
            # Just skip on win32
            return
        autotools = Autotools(self)

        if self.options.fPIC:
            autotools.fpic = True
        autotools.autoreconf()
        autotools.configure()
        autotools.make()
        autotools.install()
        # If we're building static libraries we don't want the shared around.
        if not self.options.shared:
            for filename in Path(self.package_folder).glob('**/*.so*'):
                print("Removing shared object file at {}".format(filename))
                os.remove(str(filename))
        for filename in Path(self.package_folder).glob('**/*.pc'):
            os.remove(str(filename))
        for filename in Path(self.package_folder).glob('**/*.la'):
            os.remove(str(filename))

    def package_info(self):
        if self.settings.compiler != "msvc":
            self.cpp_info.libs = ['xdg-basedir']

    def configure(self):
        # Remove since this is a pure C package.
        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd
