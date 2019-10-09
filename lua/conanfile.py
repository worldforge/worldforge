import os
import shutil

from conans import ConanFile, tools, AutoToolsBuildEnvironment


class Lua(ConanFile):
    name = 'lua'
    version = '5.1.5'
    license = 'MIT'
    url = ''
    homepage = 'https://www.lua.org/'
    description = ''
    settings = 'os', 'arch', 'build_type', 'compiler'
    generators = 'cmake'
    exports_sources = 'CMakeLists.txt'
    options = {
        'forModule': [True, False],
    }
    default_options = (
        'forModule=False',
    )

    def source(self):
        url = 'https://www.lua.org/ftp/lua-5.1.5.tar.gz'
        tools.get(url)
        luafolder = 'lua-5.1.5'
        files = os.listdir(luafolder)
        for f in files:
            shutil.move(os.path.join(luafolder, f), f)

    def build(self):
        print(self.package_folder)
        tools.replace_in_file("Makefile", "INSTALL_TOP= /usr/local", "INSTALL_TOP= {}".format(self.package_folder))
        tools.replace_in_file("src/Makefile", "CFLAGS= -O2 -Wall $(MYCFLAGS)", "CFLAGS= -O2 -Wall -fPIC $(MYCFLAGS)")

        autotools = AutoToolsBuildEnvironment(self)
        system = "linux"
        if tools.os_info.is_windows:
            system = "windows"
        elif tools.os_info.is_macos:
            system = "macosx"
        autotools.make(args=[system])
        os.rename("src/liblua.a", 'src/liblua5.1.a')

    # autotools.install()

    def package(self):
        self.copy('*lua.lib', dst='lib', keep_path=False)
        self.copy('*.dll', dst='lib', keep_path=False)
        self.copy('*.so', dst='lib', keep_path=False)
        self.copy('*.dylib', dst='lib', keep_path=False)
        self.copy('*.a', dst='lib', keep_path=False)
        headers = ['lua.h', 'luaconf.h', 'lualib.h', 'lauxlib.h', 'lua.hpp']
        for header in headers:
            self.copy(header, dst='include/lua5.1', src='src')

    def package_info(self):
        self.cpp_info.libs = ['lua5.1']
        self.cpp_info.includedirs = ["include/lua5.1"]
