find_package(rclcpp REQUIRED)

add_library(${PROJECT_NAME}-ros
  node.cpp
)
add_library(${PROJECT_NAME}::${PROJECT_NAME}-ros ALIAS ${PROJECT_NAME}-ros)

target_include_directories(${PROJECT_NAME}-ros
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(${PROJECT_NAME}-ros
  PUBLIC
  ${PROJECT_NAME}::${PROJECT_NAME}
  rclcpp::rclcpp
)

target_compile_options(${PROJECT_NAME}-ros
  PRIVATE
  ${${PROJECT_NAME}_DEFAULT_WARNING_FLAGS}
)

set_target_properties(${PROJECT_NAME}-ros PROPERTIES
  # All symbols are NO_EXPORT by default
  # CXX_VISIBILITY_PRESET hidden

  # Add the '-debug' when compiled in CMAKE_BUILD_TYPE=DEBUG
  DEBUG_POSTFIX "-debug"

  # Name used when exporting (otherwise will be named ${PROJECT_NAME}-ros)
  # EXPORT_NAME "${PROJECT_NAME}"

  # Version stuff for the export lib names, that handles symlinks
  # like:
  # libtoto.so -> libtoto.1.so -> libtoto.1.0.2.so
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  COMPATIBLE_INTERFACE_STRING ${PROJECT_VERSION_MAJOR}
)

install(TARGETS ${PROJECT_NAME}-ros
  EXPORT ${PROJECT_NAME}-ros
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ${PROJECT_NAME}-ros
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
