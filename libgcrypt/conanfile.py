#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, tools, AutoToolsBuildEnvironment


class LibgcryptConan(ConanFile):
    name = "libgcrypt"
    version = "1.8.7"
    url = "http://github.com/bincrafters/conan-libgcrypt"
    author = "Bincrafters <bincrafters@gmail.com>"
    description = "Libgcrypt is a general purpose cryptographic library originally based on code from GnuPG"
    license = "LGPL-2.1-or-later"
    homepage = "https://www.gnupg.org/download/index.html#libgcrypt"
    topics = ("conan", "libgcrypt", "gcrypt", "gnupg", "gpg", "crypto", "cryptography")
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False],
               "fPIC": [True, False]}

    default_options = {"shared": False, "fPIC": True}
    exports = ["LICENSE.md"]

    requires = 'libgpg-error/1.42@worldforge/stable'

    def source(self):
        source_url = "https://gnupg.org/ftp/gcrypt/libgcrypt"
        tools.get("{0}/libgcrypt-{1}.tar.gz".format(source_url, self.version),
                  sha256="55d98db5e5c7e7bb1efabe1299040d501e5d55272e10f60b68de9f9118b53102")

    def build(self):
        with tools.chdir("libgcrypt-{}".format(self.version)):
            autotools = AutoToolsBuildEnvironment(self)
            if self.options.fPIC:
                autotools.fpic = True
            args = (['--enable-shared', '--disable-static']
                    if self.options.shared else
                    ['--enable-static', '--disable-shared'])
            autotools.configure(args=args)
            autotools.make()
            autotools.install()

    def package(self):
        self.copy(pattern="COPYING*", src="sources", dst="licenses")

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
