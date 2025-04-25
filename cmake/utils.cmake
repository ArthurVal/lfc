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
#   * append_default_warnings_to(VAR_NAME)
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
  cmake_parse_arguments(args
    ""         # <- Flags
    "ONLY_LOG" # <- One value
    ""         # <- Multi values
    ${ARGN}
  )

  list(APPEND ALLOWED_LOG WARNING STATUS)
  if(args_ONLY_LOG AND NOT args_ONLY_LOG IN_LIST ALLOWED_LOG)
    list(JOIN ALLOWED_LOG " \n - " error_msg)
    string(PREPEND error_msg
      " Unknown ONLY_LOG argument \"${args_ONLY_LOG}\".\n"
      " Expecting one of:\n"
      " - "
    )
    message(FATAL_ERROR ${error_msg})
  endif()

  # make sure the user doesn't play dirty with simlinks
  get_filename_component(SRC_DIR "${CMAKE_SOURCE_DIR}" REALPATH)
  get_filename_component(BIN_DIR "${CMAKE_BINARY_DIR}" REALPATH)

  if("${SRC_DIR}" STREQUAL "${BIN_DIR}")
    if(args_ONLY_LOG)
      message(${args_ONLY_LOG}
        " In-source build detected.\n"
        " This will pollute your sources with build artifacts.\n"
        " We highly recommend making an independant build/ directory using:\n"
        " cmake -B <SOURCE_DIR>/build/ -S <SOURCE_DIR>\n"
      )
    else()
      message(FATAL_ERROR
        " In-source build is not allowed.\n"
        " Please make an independant build directory using:\n"
        " cmake -B <BUILD_DIR> -S <SOURCE_DIR>\n"
        " \n"
        " Feel free to:\n"
        " rm ${SRC_DIR}/CMakeCache.txt && rm -rf ${SRC_DIR}/CMakeFiles/*\n"
      )
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
  cmake_parse_arguments(args
    "ASSERTS" # <- Flags
    "DEFAULT_TO"  # <- One value
    ""         # <- Multi values
    ${ARGN}
  )

  if(NOT CMAKE_BUILD_TYPE)
    string(APPEND msg
      " CMAKE_BUILD_TYPE has not been defined. It should be set to one of: \n"
      " - None\n"
      " - Debug:          ${CMAKE_CXX_FLAGS_DEBUG}\n"
      " - Release:        ${CMAKE_CXX_FLAGS_RELEASE}\n"
      " - RelWithDebInfo: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})\n"
      " - MinSizeRel:     ${CMAKE_CXX_FLAGS_MINSIZEREL}\n"
    )

    if(args_ASSERTS)
      message(FATAL_ERROR ${msg})
    endif()

    if(NOT args_DEFAULT_TO)
      set(args_DEFAULT_TO Release)
    else()
      string(TOUPPER ${args_DEFAULT_TO} args_DEFAULT_TO)
      list(APPEND VALID_DEFAULT_TO
        NONE
        DEBUG
        RELEASE
        RELWITHDEBINFO
        MINSIZEREL
      )
      if(NOT args_DEFAULT_TO IN_LIST VALID_DEFAULT_TO)
        message(FATAL_ERROR
          "Wrong DEFAULT_TO argument ${args_DEFAULT_TO}."
          "Expecting one of: ${VALID_DEFAULT_TO}"
        )
      endif()
    endif()

    string(APPEND msg
      " ==> Automatically setting it to \"${args_DEFAULT_TO}\"."
    )
    message(WARNING ${msg})

    set(CMAKE_BUILD_TYPE
      ${args_DEFAULT_TO}
      CACHE STRING
      "Type of the build. One of None, Debug, Release, RelWithDebInfo or MinSizeRel."
      FORCE
    )
  endif()

  include(CMakePrintHelpers)
  cmake_print_variables(CMAKE_BUILD_TYPE)
endfunction()

macro(utils_append_default_warnings_to VAR_NAME)
  cmake_parse_arguments(args
    "WARNINGS_AS_ERRORS" # <- Flags
    ""                   # <- One value
    ""                   # <- Multi values
    ${ARGN}
  )

  if(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|.*Clang)$")
    if (args_WARNINGS_AS_ERRORS)
      list(APPEND ${VAR_NAME} -Werror)
    endif()

    # Warnings present on all supported versions of GCC and Clang.
    list(APPEND ${VAR_NAME}
      -Wall                # Enables most warnings.
      -Wextra              # Enables an extra set of warnings.
      -pedantic            # Strict compliance to the standard is not met.
      -Wcast-align         # Pointer casts which increase alignment.
      -Wcast-qual          # A pointer is cast to remove a type qualifier, or add an unsafe one.
      -Wconversion         # Implicit type conversions that may change a value.
      -Wformat=2           # printf/scanf/strftime/strfmon format string anomalies.
      -Wnon-virtual-dtor   # Non-virtual destructors are found.
      -Wold-style-cast     # C-style cast is used in a program.
      -Woverloaded-virtual # Overloaded virtual function names.
      -Wsign-conversion    # Implicit conversions between signed and unsigned integers.
      -Wshadow             # One variable shadows another.
      -Wswitch-enum        # A switch statement has an index of enumerated type and lacks a case.
      -Wundef              # An undefined identifier is evaluated in an #if directive.
      -Wunused             # Enable all -Wunused- warnings.
    )

    # Enable additional warnings depending on the compiler and compiler version in use.
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      list(APPEND ${VAR_NAME}
        -Wdisabled-optimization   # GCC’s optimizers are unable to handle the code effectively.
        -Wlogical-op              # Warn when a logical operator is always evaluating to true or false.
        -Wsign-promo              # Overload resolution chooses a promotion from unsigned to a signed type.
        -Wredundant-decls         # Something is declared more than once in the same scope.
      )
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.6)
        list(APPEND ${VAR_NAME}
          -Wdouble-promotion      # Warn about implicit conversions from "float" to "double".
        )
      endif ()
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 4.8)
        list(APPEND ${VAR_NAME}
          -Wuseless-cast          # Warn about useless casts.
        )
      endif ()
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 5)
        list(APPEND ${VAR_NAME}
          -Wdate-time             # Warn when encountering macros that might prevent bit-wise-identical compilations.
          -Wsuggest-final-methods # Virtual methods that could be declared final or in an anonymous namespace.
          -Wsuggest-final-types   # Types with virtual methods that can be declared final or in an anonymous namespace.
          -Wsuggest-override      # Overriding virtual functions that are not marked with the override keyword.
        )
      endif ()
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 6)
        list(APPEND ${VAR_NAME}
          -Wduplicated-cond           # Warn about duplicated conditions in an if-else-if chain.
          -Wmisleading-indentation    # Warn when indentation does not reflect the block structure.
          # -Wmultiple-inheritance      # Do not allow multiple inheritance.
          -Wnull-dereference          # Dereferencing a pointer may lead to undefined behavior.
        )
      endif ()
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 7)
        list(APPEND ${VAR_NAME}
          -Walloca                    # Warn on any usage of alloca in the code.
          -Wduplicated-branches       # Warn about duplicated branches in if-else statements.
        )
      endif ()
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 8)
        list(APPEND ${VAR_NAME}
          -Wextra-semi                # Redundant semicolons after in-class function definitions.
          -Wunsafe-loop-optimizations # The loop cannot be optimized because the compiler cannot assume anything.
        )
      endif ()
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10)
        list(APPEND ${VAR_NAME}
          -Warith-conversion          # Stricter implicit conversion warnings in arithmetic operations.
          -Wredundant-tags            # Redundant class-key and enum-key where it can be eliminated.
        )
      endif ()
    elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
      list(APPEND ${VAR_NAME}
        -Wdouble-promotion            # Warn about implicit conversions from "float" to "double".
        -Wnull-dereference            # Dereferencing a pointer may lead to erroneous or undefined behavior.
        -Wno-unknown-warning-option   # Ignore unknown warning options.
      )
    endif ()
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if (args_WARNINGS_AS_ERRORS)
      list(APPEND ${VAR_NAME} /WX)
    endif()

    list(APPEND ${VAR_NAME}
      /permissive- # Specify standards conformance mode to the compiler.
      /W4          # Enable level 4 warnings.
      /w14062      # Enumerator 'identifier' in a switch of enum 'enumeration' is not handled.
      /w14242      # The types are different, possible loss of data. The compiler makes the conversion.
      /w14254      # A larger bit field was assigned to a smaller bit field, possible loss of data.
      /w14263      # Member function does not override any base class virtual member function.
      /w14265      # 'class': class has virtual functions, but destructor is not virtual.
      /w14287      # 'operator': unsigned/negative constant mismatch.
      /w14289      # Loop control variable is used outside the for-loop scope.
      /w14296      # 'operator': expression is always false.
      /w14311      # 'variable' : pointer truncation from 'type' to 'type'.
      /w14545      # Expression before comma evaluates to a function which is missing an argument list.
      /w14546      # Function call before comma missing argument list.
      /w14547      # Operator before comma has no effect; expected operator with side-effect.
      /w14549      # Operator before comma has no effect; did you intend 'operator2'?
      /w14555      # Expression has no effect; expected expression with side-effect.
      /w14619      # #pragma warning: there is no warning number 'number'.
      /w14640      # 'instance': construction of local static object is not thread-safe.
      /w14826      # Conversion from 'type1' to 'type2' is sign-extended.
      /w14905      # Wide string literal cast to 'LPSTR'.
      /w14906      # String literal cast to 'LPWSTR'.
      /w14928      # Illegal copy-initialization; applied more than one user-defined conversion.
    )
  endif ()
endmacro()
