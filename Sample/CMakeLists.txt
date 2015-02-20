file(GLOB SOURCE_FILES "src/*.cpp" "include/clair/*.h")

include_directories("${PROJECT_SOURCE_DIR}/Clair/include")

add_executable(Sample ${SOURCE_FILES})
set_target_properties(Sample PROPERTIES DEBUG_POSTFIX _d)
target_link_libraries(Sample Clair)
