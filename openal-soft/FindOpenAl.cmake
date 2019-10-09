# Try to find OpenAL
# Once done, this will define
#
# OPENAL_FOUND        - system has OpenAL
# OPENAL_INCLUDE_DIRS - OpenAL include directories
# OPENAL_LIBRARIES    - libraries need to use OpenAL

find_path(
	OPENAL_INCLUDE_DIR
	NAMES al.h
	PATHS ${CONAN_INCLUDE_DIRS_OPENAL}
)

find_library(
	OPENAL_LIBRARY
	NAMES openal OpenAL32
	PATHS ${CONAN_LIB_DIRS_OPENAL}
)

set(OPENAL_FOUND TRUE)
set(OPENAL_INCLUDE_DIRS ${OPENAL_INCLUDE_DIR})
set(OPENAL_LIBRARIES ${OPENAL_LIBRARY})

mark_as_advanced(OPENAL_LIBRARY OPENAL_INCLUDE_DIR)
