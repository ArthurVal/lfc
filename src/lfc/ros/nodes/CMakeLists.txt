add_executable(${PROJECT_NAME}-node
  lfc.cpp
)

target_link_libraries(${PROJECT_NAME}-node
  PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}-ros
)

target_compile_options(${PROJECT_NAME}-node
  PRIVATE
  ${${PROJECT_NAME}_DEFAULT_WARNING_FLAGS}
)

set_target_properties(${PROJECT_NAME}-node PROPERTIES
  # Add the '-debug' when compiled in CMAKE_BUILD_TYPE=DEBUG
  DEBUG_POSTFIX "-debug"

  # Don't use the target name as exec name
  OUTPUT_NAME "lfc"

  # Version stuff for the export exec names, that handles symlinks
  # like:
  # libtoto.so -> libtoto.1.so -> libtoto.1.0.2.so
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  COMPATIBLE_INTERFACE_STRING ${PROJECT_VERSION_MAJOR}
)
