# .rst:
# ----- Utils.cmake -----
# This module provide global utils functionallity to the CMake project. All the
# following elements provided are prefixed by 'utils_'.
#
# ~~~
# Variables:
#   * n/a
# Functions:
#   * disable_in_source_builds([ONLY_LOG <WARNING;STATUS>])
#   * if_build_type_missing([ASSERTS][DEFAULT_TO <type>])
# Macros:
#   * n/a
# ~~~

# ~~~
# Disables in-source builds (i.e. when SOURCE_DIR == BINARY_DIR when doing, for
# example, 'cmake .' or 'cmake -S . -B .'.
#
# Arguments:
#   * ONLY_LOG <WARNING, STATUS> (optional - in):
#     Use message(${ONLY_LOG} ...) instead of FATAL_ERROR.
# ~~~
function(utils_disable_in_source_builds)
  # cmake-format: off
  cmake_parse_arguments(
    args
    ""         # <- Flags
    "ONLY_LOG" # <- One value
    ""         # <- Multi values
    ${ARGN})
  # cmake-format: on

  list(APPEND ALLOWED_LOG WARNING STATUS)
  if(args_ONLY_LOG AND NOT args_ONLY_LOG IN_LIST ALLOWED_LOG)
    list(JOIN ALLOWED_LOG " \n - " error_msg)
    # cmake-format: off
    string(PREPEND error_msg
           " Unknown ONLY_LOG argument \"${args_ONLY_LOG}\".\n"
           " Expecting one of:\n"
           " - ")
   # cmake-format: on
    message(FATAL_ERROR ${error_msg})
  endif()

  # make sure the user doesn't play dirty with simlinks
  get_filename_component(SRC_DIR "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(BIN_DIR "${CMAKE_BINARY_DIR}" REALPATH)

  if("${SRC_DIR}" STREQUAL "${BIN_DIR}")
    if(args_ONLY_LOG)
      message(
        ${args_ONLY_LOG}
        " In-source build detected.\n"
        " This will pollute your sources with build artifacts.\n"
        " We highly recommend making an independant build/ directory using:\n"
        " cmake -B <SOURCE_DIR>/build/ -S <SOURCE_DIR>\n")
    else()
      message(
        FATAL_ERROR
          " In-source build is not allowed.\n"
          " Please make an independant build directory using:\n"
          " cmake -B <BUILD_DIR> -S <SOURCE_DIR>\n"
          " \n"
          " Feel free to:\n"
          " rm ${SRC_DIR}/CMakeCache.txt && rm -rf ${SRC_DIR}/CMakeFiles/*\n")
    endif()
  endif()
endfunction()

# ~~~
# Set the default CMAKE_BUILD_TYPE and warns about it if missing.
#
# Arguments:
#   * ASSERTS (optional - in):
#     Stop CMake indicating that CMAKE_BUILD_TYPE is missing
#   * DEFAULT_TO <DEBUG,RELEASE,RELWITHDEBINFO,MINSIZEREL,NONE> (optional - in):
#     Default value (set to RELEASE if not specified).
# ~~~
function(utils_if_build_type_missing)
  # cmake-format: off
  cmake_parse_arguments(
    args
    "ASSERTS" # <- Flags
    "DEFAULT_TO"  # <- One value
    ""         # <- Multi values
    ${ARGN})
  # cmake-format: on

  if(NOT CMAKE_BUILD_TYPE)
    string(
      APPEND
      msg
      " CMAKE_BUILD_TYPE has not been defined. It should be set to one of: \n"
      " - None\n"
      " - Debug:          ${CMAKE_CXX_FLAGS_DEBUG}\n"
      " - Release:        ${CMAKE_CXX_FLAGS_RELEASE}\n"
      " - RelWithDebInfo: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})\n"
      " - MinSizeRel:     ${CMAKE_CXX_FLAGS_MINSIZEREL}\n")

    if(args_ASSERTS)
      message(FATAL_ERROR ${msg})
    endif()

    if(NOT args_DEFAULT_TO)
      set(args_DEFAULT_TO Release)
    else()
      string(TOUPPER ${args_DEFAULT_TO} args_DEFAULT_TO)
      list(
        APPEND
        VALID_DEFAULT_TO
        NONE
        DEBUG
        RELEASE
        RELWITHDEBINFO
        MINSIZEREL)
      if(NOT args_DEFAULT_TO IN_LIST VALID_DEFAULT_TO)
        message(FATAL_ERROR "Wrong DEFAULT_TO argument ${args_DEFAULT_TO}."
                            "Expecting one of: ${VALID_DEFAULT_TO}")
      endif()
    endif()

    string(APPEND msg
           " ==> Automatically setting it to \"${args_DEFAULT_TO}\".")
    message(WARNING ${msg})

    set(CMAKE_BUILD_TYPE
        ${args_DEFAULT_TO}
        CACHE STRING "Type of the build. "
              "One of None, Debug, Release, RelWithDebInfo or MinSizeRel.")
  endif()

  include(CMakePrintHelpers)
  cmake_print_variables(CMAKE_BUILD_TYPE)
endfunction()
