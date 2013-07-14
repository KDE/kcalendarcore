include(CheckIncludeFiles)
include(CheckLibraryExists)

check_include_files(uuid/uuid.h HAVE_UUID_UUID_H)
find_library(UUID_LIBRARY NAMES e2fs-uuid uuid)
if(UUID_LIBRARY)
  check_library_exists("${UUID_LIBRARY}" uuid_generate_random "${LIB_INSTALL_DIR}" HAVE_UUID_LIBRARY)
endif(UUID_LIBRARY)
