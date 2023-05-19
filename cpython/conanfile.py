import os
import re
import textwrap
from io import StringIO

from conan import ConanFile
from conan.tools.apple import is_apple_os
from conan.tools.files import get, replace_in_file, rmdir, apply_conandata_patches, export_conandata_patches, copy, \
    rename
from conan.tools.gnu import AutotoolsToolchain, AutotoolsDeps, Autotools
from conan.tools.layout import basic_layout
from conan.tools.microsoft import is_msvc
from conans.errors import ConanInvalidConfiguration
from conans.model.version import Version
from conans.util.env import get_env
from hgext.githelp import mv

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
        "with_bz2": [True, False],
        "with_gdbm": [True, False],
        "with_nis": [True, False],
        "with_sqlite3": [True, False],
        "with_tkinter": [True, False],
        "with_curses": [True, False],

        # Python 2 options
        "unicode": ["ucs2", "ucs4"],
        "with_bsddb": [True, False],
        # Python 3 options
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
        "with_bz2": False,
        "with_gdbm": False,
        "with_nis": False,
        "with_sqlite3": False,
        "with_tkinter": False,
        "with_curses": False,

        # Python 2 options
        "unicode": "ucs2",
        "with_bsddb": False,  # True,  # FIXME: libdb package missing (#5309/#5392)
        # Python 3 options
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

    @property
    def _is_py2(self):
        return Version(self._version_number_only).major == "2"

    def config_options(self):
        if self._is_py2:
            # Python 2.xx does not support following options
            del self.options.with_lzma
        elif self._is_py3:
            # Python 3.xx does not support following options
            del self.options.with_bsddb
            del self.options.unicode

        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd

    def configure(self):
        if self.options.shared:
            del self.options.fPIC
        if not self._supports_modules:
            del self.options.with_bz2
            del self.options.with_sqlite3
            del self.options.with_tkinter

            del self.options.with_bsddb
            del self.options.with_lzma

    def validate(self):
        if self.options.get_safe("with_curses", False) and not self.options["ncurses"].with_widec:
            raise ConanInvalidConfiguration("cpython requires ncurses with wide character support")

    def package_id(self):
        del self.info.options.env_vars

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    @property
    def _with_libffi(self):
        return self._supports_modules

    def requirements(self):
        self.requires("zlib/1.2.12")
        if self._supports_modules:
            self.requires("expat/2.4.1")
            if self._with_libffi:
                self.requires("libffi/3.4.4")
            self.requires("mpdecimal/2.5.0")  # FIXME: no 2.5.1 to troubleshoot apple
        if self.settings.os != "Windows":
            if not is_apple_os(self):
                self.requires("libuuid/1.0.3")
            self.requires("libxcrypt/4.4.25")
        if self.options.get_safe("with_bz2"):
            self.requires("bzip2/1.0.8")
        if self.options.get_safe("with_gdbm", False):
            self.requires("gdbm/1.19")
        if self.options.get_safe("with_nis", False):
            # TODO: Add nis when available.
            raise ConanInvalidConfiguration("nis is not available on CCI (yet)")
        if self.options.get_safe("with_sqlite3"):
            self.requires("sqlite3/3.36.0")
        if self.options.get_safe("with_tkinter"):
            self.requires("tk/8.6.10")
        if self.options.get_safe("with_curses", False):
            self.requires("ncurses/6.2")
        if self.options.get_safe("with_bsddb", False):
            self.requires("libdb/5.3.28")
        if self.options.get_safe("with_lzma", False):
            self.requires("xz_utils/5.2.5")

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
        if self._is_py2:
            tc.configure_args.extend([
                "--enable-unicode={}".format(yes_no(self.options.unicode)),
            ])
        if self._is_py3:
            tc.configure_args.extend([
                "--with-system-libmpdec"
            ])
            if self.options.get_safe("with_sqlite3"):
                tc.configure_args.extend([
                    "--enable-loadable-sqlite-extensions={}".format(
                        yes_no(not self.options["sqlite3"].omit_load_extension)),
                ])
        if self.settings.compiler == "intel-cc":
            tc.configure_args.extend(["--with-icc"])
        if get_env("CC") or self.settings.compiler != "gcc":
            tc.configure_args.append("--without-gcc")
        if self.options.with_tkinter:
            tcltk_includes = []
            tcltk_libs = []
            # FIXME: collect using some conan util (https://github.com/conan-io/conan/issues/7656)
            for dep in ("tcl", "tk", "zlib"):
                tcltk_includes += ["-I{}".format(d) for d in self.deps_cpp_info[dep].include_paths]
                tcltk_libs += ["-l{}".format(lib) for lib in self.deps_cpp_info[dep].libs]
            if self.settings.os == "Linux" and not self.options["tk"].shared:
                # FIXME: use info from xorg.components (x11, xscrnsaver)
                tcltk_libs.extend(["-l{}".format(lib) for lib in ("X11", "Xss")])
            tc.configure_args.extend([
                "--with-tcltk-includes={}".format(" ".join(tcltk_includes)),
                "--with-tcltk-libs={}".format(" ".join(tcltk_libs)),
            ])

        if self.settings.os in ("Linux", "FreeBSD"):
            # Building _testembed fails due to missing pthread/rt symbols
            tc.extra_ldflags.append("-lpthread")

        tc.generate()

        deps = AutotoolsDeps(self)
        deps.generate()

    def _patch_sources(self):
        apply_conandata_patches(self)
        if self._is_py3 and Version(self._version_number_only) < "3.10":
            replace_in_file(self, os.path.join(self.source_folder, "setup.py"),
                            ":libmpdec.so.2", "mpdec")

        # Remove vendored packages
        rmdir(self, os.path.join(self.source_folder, "Modules", "_decimal", "libmpdec"))
        rmdir(self, os.path.join(self.source_folder, "Modules", "expat"))

        if self.options.get_safe("with_curses", False):
            # FIXME: this will link to ALL libraries of ncurses. Only need to link to ncurses(w) (+ eventually tinfo)
            replace_in_file(self, os.path.join(self.source_folder, "setup.py"),
                            "curses_libs = ",
                            "curses_libs = {} #".format(repr(
                                self.deps_cpp_info["ncurses"].libs + self.deps_cpp_info["ncurses"].system_libs)))

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
        if self._is_py3:
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
        if self.settings.os != "Windows":
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
        self.cpp_info.components["_embed_copy"].set_property("pkg_config_name", "python{}-embed".format(py_version.major))
        self.cpp_info.components["_embed_copy"].libdirs = []

        if self._supports_modules:
            # hidden components: the C extensions of python are built as dynamically loaded shared libraries.
            # C extensions or applications with an embedded Python should not need to link to them..
            self.cpp_info.components["_hidden"].requires = [
                "expat::expat",
                "mpdecimal::mpdecimal",
            ]
            if self._with_libffi:
                self.cpp_info.components["_hidden"].requires.append("libffi::libffi")
            if self.settings.os != "Windows":
                if not is_apple_os(self):
                    self.cpp_info.components["_hidden"].requires.append("libuuid::libuuid")
                self.cpp_info.components["_hidden"].requires.append("libxcrypt::libxcrypt")
            if self.options.with_bz2:
                self.cpp_info.components["_hidden"].requires.append("bzip2::bzip2")
            if self.options.get_safe("with_gdbm", False):
                self.cpp_info.components["_hidden"].requires.append("gdbm::gdbm")
            if self.options.with_sqlite3:
                self.cpp_info.components["_hidden"].requires.append("sqlite3::sqlite3")
            if self.options.get_safe("with_curses", False):
                self.cpp_info.components["_hidden"].requires.append("ncurses::ncurses")
            if self.options.get_safe("with_bsddb"):
                self.cpp_info.components["_hidden"].requires.append("libdb::libdb")
            if self.options.get_safe("with_lzma"):
                self.cpp_info.components["_hidden"].requires.append("xz_utils::xz_utils")
            if self.options.get_safe("with_tkinter"):
                self.cpp_info.components["_hidden"].requires.append("tk::tk")
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

        if self._is_py2:
            python_root = ""
        else:
            python_root = self.package_folder
            if self.options.env_vars:
                self.output.info("Setting PYTHON_ROOT environment variable: {}".format(python_root))
                self.runenv_info.define("PYTHON_ROOT", python_root)
        self.conf_info.define("user.cpython:python_root", python_root)
