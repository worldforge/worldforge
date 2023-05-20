import os
import re
import textwrap
from io import StringIO

from conan import ConanFile
from conan.tools.apple import is_apple_os
from conan.tools.files import get, replace_in_file, rmdir, apply_conandata_patches, export_conandata_patches, copy
from conan.tools.gnu import AutotoolsToolchain, AutotoolsDeps, Autotools
from conan.tools.layout import basic_layout
from conans.errors import ConanInvalidConfiguration
from conans.model.version import Version
from conans.util.env import get_env

required_conan_version = ">=2.0.0"


class CPythonConan(ConanFile):
    name = "cpython"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://www.python.org"
    description = "Python is a programming language that lets you work quickly and integrate systems more effectively."
    topics = ("python", "cpython", "language", "script")
    license = ("Python-2.0",)
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "optimizations": [True, False],
        "lto": [True, False],
        "docstrings": [True, False],
        "pymalloc": [True, False],
        "with_lzma": [True, False],

        # options that don't change package id
        "env_vars": [True, False],  # set environment variables
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "optimizations": False,
        "lto": False,
        "docstrings": True,
        "pymalloc": True,
        "with_lzma": False,

        # options that don't change package id
        "env_vars": True,
    }
    user = "worldforge"
    version = "3.10.0"
    package_type = "library"

    def layout(self):
        basic_layout(self, src_folder="src")

    def export_sources(self):
        export_conandata_patches(self)

    @property
    def _version_number_only(self):
        return re.match(r"^([0-9.]+)", self.version).group(1)

    @property
    def _version_tuple(self):
        return tuple(self._version_number_only.split("."))

    @property
    def _supports_modules(self):
        return True

    @property
    def _version_suffix(self):
        joiner = "."
        return joiner.join(self._version_tuple[:2])

    @property
    def _is_py3(self):
        return Version(self._version_number_only).major == "3"


    def config_options(self):
        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd

    def configure(self):
        if self.options.shared:
            del self.options.fPIC

    def package_id(self):
        del self.info.options.env_vars

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def requirements(self):
        self.requires("zlib/1.2.13")
        if self._supports_modules:
            self.requires("expat/2.5.0")
            self.requires("libffi/3.4.4")
            self.requires("mpdecimal/2.5.0")
        if not is_apple_os(self):
            self.requires("libuuid/1.0.3")
        self.requires("libxcrypt/4.4.25")

    def generate(self):
        yes_no = lambda v: "yes" if v else "no"
        tc = AutotoolsToolchain(self)
        tc.configure_args.extend([
            "--prefix={}".format(self.package_folder),
            "--enable-shared={}".format(yes_no(self.options.shared)),
            "--with-doc-strings={}".format(yes_no(self.options.docstrings)),
            "--with-pymalloc={}".format(yes_no(self.options.pymalloc)),
            "--with-system-expat",
            "--with-system-ffi",
            "--enable-optimizations={}".format(yes_no(self.options.optimizations)),
            "--with-lto={}".format(yes_no(self.options.lto)),
            "--with-pydebug={}".format(yes_no(self.settings.build_type == "Debug")),
        ])
        tc.configure_args.extend([
            "--with-system-libmpdec"
        ])
        if self.settings.compiler == "intel-cc":
            tc.configure_args.extend(["--with-icc"])
        if get_env("CC") or self.settings.compiler != "gcc":
            tc.configure_args.append("--without-gcc")


        if self.settings.os in ("Linux", "FreeBSD"):
            # Building _testembed fails due to missing pthread/rt symbols
            tc.extra_ldflags.append("-lpthread")

        tc.generate()

        deps = AutotoolsDeps(self)
        deps.generate()

    def _patch_sources(self):
        apply_conandata_patches(self)

        # Remove vendored packages
        rmdir(self, os.path.join(self.source_folder, "Modules", "_decimal", "libmpdec"))
        rmdir(self, os.path.join(self.source_folder, "Modules", "expat"))

    def build(self):
        self._patch_sources()

        autotools = Autotools(self)
        autotools.configure()
        autotools.make()

    def package(self):
        copy(self, "LICENSE", src=self.source_folder, dst="licenses")

        autotools = Autotools(self)
        autotools.install(["DESTDIR=/"])

        rmdir(self, os.path.join(self.package_folder, "lib", "pkgconfig"))
        rmdir(self, os.path.join(self.package_folder, "share"))

        # Rewrite shebangs of python scripts
        for filename in os.listdir(os.path.join(self.package_folder, "bin")):
            filepath = os.path.join(self.package_folder, "bin", filename)
            if not os.path.isfile(filepath):
                continue
            if os.path.islink(filepath):
                continue
            with open(filepath, "rb") as fn:
                firstline = fn.readline(1024)
                if not (firstline.startswith(b"#!") and b"/python" in firstline and b"/bin/sh" not in firstline):
                    continue
                text = fn.read()
            self.output.info("Rewriting shebang of {}".format(filename))
            with open(filepath, "wb") as fn:
                fn.write(textwrap.dedent("""\
                    #!/bin/sh
                    ''':'
                    __file__="$0"
                    while [ -L "$__file__" ]; do
                        __file__="$(dirname "$__file__")/$(readlink "$__file__")"
                    done
                    exec "$(dirname "$__file__")/python{}" "$0" "$@"
                    '''
                    """.format(self._version_suffix)).encode())
                fn.write(text)

        if not os.path.exists(self._cpython_symlink):
            os.symlink("python{}".format(self._version_suffix), self._cpython_symlink)
        self._fix_install_name()

    @property
    def _cpython_symlink(self):
        symlink = os.path.join(self.package_folder, "bin", "python")
        if self.settings.os == "Windows":
            symlink += ".exe"
        return symlink

    @property
    def _cpython_interpreter_name(self):
        suffix = self._version_suffix
        python = "python{}".format(suffix)
        return python

    @property
    def _cpython_interpreter_path(self):
        return os.path.join(self.package_folder, "bin", self._cpython_interpreter_name)

    @property
    def _abi_suffix(self):
        res = ""
        if self.settings.build_type == "Debug":
            res += "d"
        if Version(self._version_number_only) < "3.8":
            if self.options.get_safe("pymalloc", False):
                res += "m"
        return res

    @property
    def _lib_name(self):
        lib_ext = self._abi_suffix + (".dll.a" if self.options.shared and self.settings.os == "Windows" else "")
        return "python{}{}".format(self._version_suffix, lib_ext)

    def _fix_install_name(self):
        if is_apple_os(self) and self.options.shared:
            buffer = StringIO()
            python = os.path.join(self.package_folder, "bin", "python")
            self.run('otool -L "%s"' % python, output=buffer)
            lines = buffer.getvalue().strip().split('\n')[1:]
            for line in lines:
                library = line.split()[0]
                if library.startswith(self.package_folder):
                    new = library.replace(self.package_folder, "@executable_path/..")
                    self.output.info("patching {}, replace {} with {}".format(python, library, new))
                    self.run("install_name_tool -change {} {} {}".format(library, new, python))

    def package_info(self):
        py_version = Version(self._version_number_only)
        # python component: "Build a C extension for Python"
        self.cpp_info.set_property("cmake_file_name", "Python3")
        self.cpp_info.set_property("cmake_target_name", "Python3::Python")
        self.cpp_info.defines.append("PYTHONHOME={}".format(self.package_folder))

        self.cpp_info.components["python"].includedirs.append(
            os.path.join("include", "python{}{}".format(self._version_suffix, self._abi_suffix)))
        libdir = "lib"
        if self.options.shared:
            self.cpp_info.components["python"].defines.append("Py_ENABLE_SHARED")
        else:
            self.cpp_info.components["python"].defines.append("Py_NO_ENABLE_SHARED")
            if self.settings.os == "Linux":
                self.cpp_info.components["python"].system_libs.extend(["dl", "m", "pthread", "util"])
        self.cpp_info.components["python"].requires = ["zlib::zlib"]
        self.cpp_info.components["python"].requires.append("libxcrypt::libxcrypt")
        self.cpp_info.components["python"].set_property("pkg_config_name", "python-{}.{}".format(py_version.major,
                                                                                                 py_version.minor))
        self.cpp_info.components["python"].libdirs = []

        self.cpp_info.components["_python_copy"].set_property("pkg_config_name", "python{}".format(py_version.major))
        self.cpp_info.components["_python_copy"].requires = ["python"]
        self.cpp_info.components["_python_copy"].libdirs = []

        # embed component: "Embed Python into an application"
        self.cpp_info.components["embed"].libs = [self._lib_name]
        self.cpp_info.components["embed"].libdirs = [libdir]
        self.cpp_info.components["embed"].set_property("pkg_config_name", "python-{}.{}-embed".format(py_version.major,
                                                                                                      py_version.minor))
        self.cpp_info.components["embed"].requires = ["python"]

        self.cpp_info.components["_embed_copy"].requires = ["embed"]
        self.cpp_info.components["_embed_copy"].set_property("pkg_config_name",
                                                             "python{}-embed".format(py_version.major))
        self.cpp_info.components["_embed_copy"].libdirs = []

        if self._supports_modules:
            # hidden components: the C extensions of python are built as dynamically loaded shared libraries.
            # C extensions or applications with an embedded Python should not need to link to them..
            self.cpp_info.components["_hidden"].requires = [
                "expat::expat",
                "mpdecimal::mpdecimal",
                "libffi::libffi"
            ]
            if not is_apple_os(self):
                self.cpp_info.components["_hidden"].requires.append("libuuid::libuuid")
            self.cpp_info.components["_hidden"].requires.append("libxcrypt::libxcrypt")
            self.cpp_info.components["_hidden"].libdirs = []

        if self.options.env_vars:
            bindir = os.path.join(self.package_folder, "bin")
            self.output.info("Appending PATH environment variable: {}".format(bindir))
            self.runenv_info.append_path("PATH", bindir)

        python = self._cpython_interpreter_path
        self.conf_info.define("user.cpython:python", python)
        if self.options.env_vars:
            self.output.info("Setting PYTHON environment variable: {}".format(python))
            self.runenv_info.define("PYTHON", python)

        if is_apple_os(self):
            pythonhome = self.package_folder
        else:
            version = Version(self._version_number_only)
            pythonhome = os.path.join(self.package_folder, "lib", "python{}.{}".format(version.major, version.minor))
        self.conf_info.define("user.cpython:pythonhome", pythonhome)

        pythonhome_required = is_apple_os(self)
        self.conf_info.define("user.cpython:module_requires_pythonhome", pythonhome_required)


        python_root = self.package_folder
        if self.options.env_vars:
            self.output.info("Setting PYTHON_ROOT environment variable: {}".format(python_root))
            self.runenv_info.define("PYTHON_ROOT", python_root)
        self.conf_info.define("user.cpython:python_root", python_root)
