cmake_minimum_required (VERSION 2.8)
project ("Systems Programming Assignment 2")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
# set(CMAKE_VERBOSE_MAKEFILE 1) # to debug the cmake generation

if(NOT CMAKE_BUILD_TYPE)
  set(
    CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
      FORCE
  )
endif()

# to use clang use -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
set(SERVER_BINARY_NAME "server")
set(CLIENT_BINARY_NAME "client")

# normal compiler flags
add_definitions(
  -Wall
  -Wextra
  -std=gnu99
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O2")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_definitions(
    -Wdocumentation
    -fdiagnostics-show-template-tree
    -fdiagnostics-print-source-range-info
    -fparse-all-comments
    -Wno-c++98-compat
  )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_definitions(
    -Wno-unknown-pragmas
  )
endif()

#### This will have to be improved
# add the source of the test entry point
# this will have to be replaced by what ever else
set(
  COMMON_SOURCES
)
set(
  SERVER_SOURCES
)
set(
  CLIENT_SOURCES
)

find_package(Threads REQUIRED)

set(
  SERVER_LIBRARIES
    ${CMAKE_THREAD_LIBS_INIT}
)

# include root directory for path correct includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# include include directory because we are lazy
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# add add_subdirectory - they will populate the SOURCES variable
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(
  SERVER_SOURCES
    ${SERVER_SOURCES}
    ${COMMON_SOURCES}
)

set(
  CLIENT_SOURCES
    ${CLIENT_SOURCES}
    ${COMMON_SOURCES}
)

#########################################################################
#                            SERVER DEFAULT                             #
#########################################################################
# create executable target - server
add_executable(server ${SERVER_SOURCES})
# specify which libraries to link to the main target
target_link_libraries(server ${SERVER_LIBRARIES})
#rename binary file
add_custom_command(
  TARGET
    server
  POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      ARGS -E rename
        ${CMAKE_CURRENT_SOURCE_DIR}/server
        ${CMAKE_CURRENT_SOURCE_DIR}/${SERVER_BINARY_NAME}
)

#########################################################################
#                       ADDRESS SANITIZER TARGET                        #
#########################################################################
# create executable target - debug_address
add_executable(
  server_debug_address
  EXCLUDE_FROM_ALL
    ${SERVER_SOURCES}
)
# specify which libraries to link to the main target
target_link_libraries(
  server_debug_address
    ${SERVER_LIBRARIES}
)
# custom target to build with clangs asan
set(
  SERVER_DEBUG_ADDRESS_FLAGS
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
    -fsanitize=address
)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(
    SERVER_DEBUG_ADDRESS_FLAGS
      ${SERVER_DEBUG_ADDRESS_FLAGS}
      -fsanitize=null
      -fsanitize=undefined-trap
      -fsanitize=unreachable
  )
endif()
# replace ';' with ' ' because cmake is unable to do it itself
string(
  REPLACE
    ";"
    " "
    SERVER_DEBUG_ADDRESS_FLAGS
    "${SERVER_DEBUG_ADDRESS_FLAGS}"
)
# set compiler flags
set_target_properties(
  server_debug_address
    PROPERTIES
      APPEND_STRING
      PROPERTY
    COMPILE_FLAGS
      ${SERVER_DEBUG_ADDRESS_FLAGS}
)
# set linker flags (this has to be done extra since the two are completely
# separated)
set_target_properties(
  server_debug_address
    PROPERTIES
      APPEND_STRING
      PROPERTY
    LINK_FLAGS
      ${SERVER_DEBUG_ADDRESS_FLAGS}
)
add_custom_command(
  TARGET
    server_debug_address
  PRE_BUILD
    COMMAND
      ASAN_OPTIONS="detect_leaks=1" # Not tested if it works
)
#rename binary file
add_custom_command(
  TARGET
    server_debug_address
  POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      ARGS -E rename
        ${CMAKE_CURRENT_SOURCE_DIR}/server_debug_address
        ${CMAKE_CURRENT_SOURCE_DIR}/${SERVER_BINARY_NAME}
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #########################################################################
  #                       MEMORY SANITIZER TARGET                         #
  #########################################################################
  # create executable target - server_debug_memory
  add_executable(
    server_debug_memory
    EXCLUDE_FROM_ALL
      ${SERVER_SOURCES}
  )
  # specify which libraries to link to the main target
  target_link_libraries(
    server_debug_memory
      ${SERVER_LIBRARIES}
  )
  # custom target to build with clangs msan
  set(
    SERVER_DEBUG_MEMORY_FLAGS
      -fno-omit-frame-pointer
      -fno-optimize-sibling-calls
      -fsanitize-memory-track-origins
      -fsanitize=memory
  )
  # replace ';' with ' ' because cmake is unable to do it itself
  string(
    REPLACE
      ";"
      " "
      SERVER_DEBUG_MEMORY_FLAGS
      "${SERVER_DEBUG_MEMORY_FLAGS}"
  )
  # set compiler flags
  set_target_properties(
    server_debug_memory
      PROPERTIES
        APPEND_STRING
        PROPERTY
      COMPILE_FLAGS
        ${SERVER_DEBUG_MEMORY_FLAGS}
  )
  # set linker flags (this has to be done extra since the two are completely
  # separated)
  set_target_properties(
    server_debug_memory
      PROPERTIES
        APPEND_STRING
        PROPERTY
      LINK_FLAGS
        ${SERVER_DEBUG_MEMORY_FLAGS}
  )
  #rename binary file
  add_custom_command(
    TARGET
      server_debug_memory
    POST_BUILD
      COMMAND ${CMAKE_COMMAND}
        ARGS -E rename
          ${CMAKE_CURRENT_SOURCE_DIR}/server_debug_memory
          ${CMAKE_CURRENT_SOURCE_DIR}/${SERVER_BINARY_NAME}
  )


  #########################################################################
  #                       THREAD SANITIZER TARGET                         #
  #########################################################################
  # create executable target - server_debug_thread
  add_executable(
    server_debug_thread
    EXCLUDE_FROM_ALL
      ${SERVER_SOURCES}
  )
  # specify which libraries to link to the main target
  target_link_libraries(
    server_debug_thread
      ${SERVER_LIBRARIES}
  )
  # custom target to build with clangs tsan
  set(
    SERVER_DEBUG_THREAD_FLAGS_COMPILER
      -fPIE
      -fsanitize=thread
  )
  set(
    SERVER_DEBUG_THREAD_FLAGS_LINKER
      ${SERVER_DEBUG_THREAD_FLAGS_COMPILER}
      -pie
  )
  # replace ';' with ' ' because cmake is unable to do it itself
  string(
    REPLACE
      ";"
      " "
      SERVER_DEBUG_THREAD_FLAGS_COMPILER
      "${SERVER_DEBUG_THREAD_FLAGS_COMPILER}"
  )
  string(
    REPLACE
      ";"
      " "
      SERVER_DEBUG_THREAD_FLAGS_LINKER
      "${SERVER_DEBUG_THREAD_FLAGS_LINKER}"
  )
  # set compiler flags
  set_target_properties(
    server_debug_thread
      PROPERTIES
        APPEND_STRING
        PROPERTY
      COMPILE_FLAGS
        ${SERVER_DEBUG_THREAD_FLAGS_COMPILER}
  )
  # set linker flags (this has to be done extra since the two are completely
  # separated)
  set_target_properties(
    server_debug_thread
      PROPERTIES
        APPEND_STRING
        PROPERTY
      LINK_FLAGS
        ${SERVER_DEBUG_THREAD_FLAGS_LINKER}
  )
  #rename binary file
  add_custom_command(
    TARGET
      server_debug_thread
    POST_BUILD
      COMMAND ${CMAKE_COMMAND}
        ARGS -E rename
          ${CMAKE_CURRENT_SOURCE_DIR}/server_debug_thread
          ${CMAKE_CURRENT_SOURCE_DIR}/${SERVER_BINARY_NAME}
  )
endif()

#########################################################################
#                               DEFAULT                                 #
#########################################################################
# create executable target - client
add_executable(client ${CLIENT_SOURCES})
#rename binary file
add_custom_command(
  TARGET
    client
  POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      ARGS -E rename
        ${CMAKE_CURRENT_SOURCE_DIR}/client
        ${CMAKE_CURRENT_SOURCE_DIR}/${CLIENT_BINARY_NAME}
)

#########################################################################
#                       ADDRESS SANITIZER TARGET                        #
#########################################################################
# create executable target - debug_address
add_executable(
  client_debug_address
  EXCLUDE_FROM_ALL
    ${CLIENT_SOURCES}
)
# custom target to build with clangs asan
set(
  CLIENT_DEBUG_ADDRESS_FLAGS
    -fno-omit-frame-pointer
    -fno-optimize-sibling-calls
    -fsanitize=address
)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(
    CLIENT_DEBUG_ADDRESS_FLAGS
      ${CLIENT_DEBUG_ADDRESS_FLAGS}
      -fsanitize=null
      -fsanitize=undefined-trap
      -fsanitize=unreachable
  )
endif()
# replace ';' with ' ' because cmake is unable to do it itself
string(
  REPLACE
    ";"
    " "
    CLIENT_DEBUG_ADDRESS_FLAGS
    "${CLIENT_DEBUG_ADDRESS_FLAGS}"
)
# set compiler flags
set_target_properties(
  client_debug_address
    PROPERTIES
      APPEND_STRING
      PROPERTY
    COMPILE_FLAGS
      ${CLIENT_DEBUG_ADDRESS_FLAGS}
)
# set linker flags (this has to be done extra since the two are completely
# separated)
set_target_properties(
  client_debug_address
    PROPERTIES
      APPEND_STRING
      PROPERTY
    LINK_FLAGS
      ${CLIENT_DEBUG_ADDRESS_FLAGS}
)
add_custom_command(
  TARGET
    client_debug_address
  PRE_BUILD
    COMMAND
      ASAN_OPTIONS="detect_leaks=1" # Not tested if it works
)
#rename binary file
add_custom_command(
  TARGET
    client_debug_address
  POST_BUILD
    COMMAND ${CMAKE_COMMAND}
      ARGS -E rename
        ${CMAKE_CURRENT_SOURCE_DIR}/client_debug_address
        ${CMAKE_CURRENT_SOURCE_DIR}/${CLIENT_BINARY_NAME}
)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  #########################################################################
  #                       MEMORY SANITIZER TARGET                         #
  #########################################################################
  # create executable target - client_debug_memory
  add_executable(
    client_debug_memory
    EXCLUDE_FROM_ALL
      ${CLIENT_SOURCES}
  )
  # custom target to build with clangs msan
  set(
    CLIENT_DEBUG_MEMORY_FLAGS
      -fno-omit-frame-pointer
      -fno-optimize-sibling-calls
      -fsanitize-memory-track-origins
      -fsanitize=memory
  )
  # replace ';' with ' ' because cmake is unable to do it itself
  string(
    REPLACE
      ";"
      " "
      CLIENT_DEBUG_MEMORY_FLAGS
      "${CLIENT_DEBUG_MEMORY_FLAGS}"
  )
  # set compiler flags
  set_target_properties(
    client_debug_memory
      PROPERTIES
        APPEND_STRING
        PROPERTY
      COMPILE_FLAGS
        ${CLIENT_DEBUG_MEMORY_FLAGS}
  )
  # set linker flags (this has to be done extra since the two are completely
  # separated)
  set_target_properties(
    client_debug_memory
      PROPERTIES
        APPEND_STRING
        PROPERTY
      LINK_FLAGS
        ${CLIENT_DEBUG_MEMORY_FLAGS}
  )
  #rename binary file
  add_custom_command(
    TARGET
      client_debug_memory
    POST_BUILD
      COMMAND ${CMAKE_COMMAND}
        ARGS -E rename
          ${CMAKE_CURRENT_SOURCE_DIR}/client_debug_memory
          ${CMAKE_CURRENT_SOURCE_DIR}/${CLIENT_BINARY_NAME}
  )


  #########################################################################
  #                       THREAD SANITIZER TARGET                         #
  #########################################################################
  # create executable target - client_debug_thread
  add_executable(
    client_debug_thread
    EXCLUDE_FROM_ALL
      ${CLIENT_SOURCES}
  )
  # custom target to build with clangs tsan
  set(
    CLIENT_DEBUG_THREAD_FLAGS_COMPILER
      -fPIE
      -fsanitize=thread
  )
  set(
    CLIENT_DEBUG_THREAD_FLAGS_LINKER
      ${CLIENT_DEBUG_THREAD_FLAGS_COMPILER}
      -pie
  )
  # replace ';' with ' ' because cmake is unable to do it itself
  string(
    REPLACE
      ";"
      " "
      CLIENT_DEBUG_THREAD_FLAGS_COMPILER
      "${CLIENT_DEBUG_THREAD_FLAGS_COMPILER}"
  )
  string(
    REPLACE
      ";"
      " "
      CLIENT_DEBUG_THREAD_FLAGS_LINKER
      "${CLIENT_DEBUG_THREAD_FLAGS_LINKER}"
  )
  # set compiler flags
  set_target_properties(
    client_debug_thread
      PROPERTIES
        APPEND_STRING
        PROPERTY
      COMPILE_FLAGS
        ${CLIENT_DEBUG_THREAD_FLAGS_COMPILER}
  )
  # set linker flags (this has to be done extra since the two are completely
  # separated)
  set_target_properties(
    client_debug_thread
      PROPERTIES
        APPEND_STRING
        PROPERTY
      LINK_FLAGS
        ${CLIENT_DEBUG_THREAD_FLAGS_LINKER}
  )
  #rename binary file
  add_custom_command(
    TARGET
      client_debug_thread
    POST_BUILD
      COMMAND ${CMAKE_COMMAND}
        ARGS -E rename
          ${CMAKE_CURRENT_SOURCE_DIR}/client_debug_thread
          ${CMAKE_CURRENT_SOURCE_DIR}/${CLIENT_BINARY_NAME}
  )
endif()


find_package(Doxygen)
if(DOXYGEN_FOUND)
  # configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/misc/Doxyfile @ONLY)
  add_custom_target(
    documentation
      ${DOXYGEN_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/misc/Doxyfile
      WORKING_DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT
        "Generating API documentation with Doxygen"
      VERBATIM
  )
endif()
