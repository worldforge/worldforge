add_library(cyphesis-external-pycxx
        Src/cxx_exceptions.cxx
        Src/cxx_extensions.cxx
        Src/cxxextensions.c
        Src/cxxsupport.cxx
        Src/IndirectPythonInterface.cxx

        CXX/Config.hxx
        CXX/cxx_standard_exceptions.hxx
        CXX/CxxDebug.hxx
        CXX/Exception.hxx
        CXX/ExtensionModule.hxx
        CXX/ExtensionOldType.hxx
        CXX/Extensions.hxx
        CXX/ExtensionType.hxx
        CXX/ExtensionTypeBase.hxx
        CXX/IndirectPythonInterface.hxx
        CXX/Objects.hxx
        CXX/PythonType.hxx
        CXX/Version.hxx
        CXX/WrapPython.h
        )

target_include_directories(cyphesis-external-pycxx PRIVATE .)
target_link_libraries(cyphesis-external-pycxx PUBLIC
        Python3::Python
        )


