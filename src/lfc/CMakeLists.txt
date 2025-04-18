add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# target_link_libraries(${PROJECT_NAME}
#   INTERFACE
# )

target_compile_features(${PROJECT_NAME}
  INTERFACE
  cxx_std_17
)

# target_compile_definitions(${PROJECT_NAME}
#   INTERFACE
#   )

# target_compile_options(${PROJECT_NAME}
#   INTERFACE
#   -Wall
#   -Wextra
#   -Wpedantic
#   -Wshadow
#   -Wdouble-promotion
#   -Wundef
#   -Wconversion
#   # -Werror
# )

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  INTERFACE_${PROJECT_NAME}_VERSION ${PROJECT_VERSION}
  COMPATIBLE_INTERFACE_STRING ${PROJECT_VERSION_MAJOR}
)


# install(
#   TARGETS ${PROJECT_NAME}
#   EXPORT ${PROJECT_NAME}Core
#   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

# install(
#   EXPORT ${PROJECT_NAME}Core
#   NAMESPACE ${PROJECT_NAME}::
#   DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
# )
