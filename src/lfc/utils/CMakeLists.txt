add_library(${PROJECT_NAME}-utils INTERFACE)
add_library(${PROJECT_NAME}::utils ALIAS ${PROJECT_NAME}-utils)

target_include_directories(${PROJECT_NAME}-utils
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>

  # PRIVATE
  # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  # $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_compile_features(${PROJECT_NAME}-utils
  INTERFACE
  cxx_std_17
)

target_compile_options(${PROJECT_NAME}-utils
  INTERFACE
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wdouble-promotion
  -Wundef
  -Wconversion
  # -Werror
)
