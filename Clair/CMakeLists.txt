file(GLOB SOURCE_FILES "src/*.cpp" "include/clair/*.h")

include_directories("${PROJECT_SOURCE_DIR}/Clair/include")

add_library(Clair STATIC ${SOURCE_FILES})
set_target_properties(Clair PROPERTIES DEBUG_POSTFIX _d)
