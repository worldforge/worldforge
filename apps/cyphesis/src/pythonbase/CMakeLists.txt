add_library(cyphesis-pythonbase
        PythonClass.cpp
        PythonContext.cpp
        Python_API.cpp
        WrapperBase.cpp
        PythonMalloc.cpp
        PythonDebug.cpp)

target_link_libraries(cyphesis-pythonbase PUBLIC
        cyphesis-common
        cyphesis-external-pycxx
)



