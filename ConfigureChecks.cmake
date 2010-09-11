include(CheckIncludeFiles)
include(CheckLibraryExists)

check_include_files(uuid/uuid.h HAVE_UUID_UUID_H)
check_library_exists(uuid uuid_generate_random "" HAVE_UUID_LIBRARY)


