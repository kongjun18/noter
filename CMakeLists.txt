cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(noter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# enable compile database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


message(STATUS "The C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "The C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "The C++ Compiler Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "The C++ Compiler Flags in Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "The C++ Compiler Flags in Release: ${CMAKE_CXX_FLAGS_RELEASe}")
message(STATUS "The C++ Compiler Flags in RelWithDebInfo: ${CMAKE_CXX_FLAGS_RELWITHDEBInfo}")
message(STATUS "The C++ Compiler Flags in MinSizeRel: ${CMAKE_CXX_FLAGS_MINSIZEREL}")
message(STATUS "The linker flags: ${CMAKE_EXE_LINKER_FLAGS}")

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# find browser
find_program(
    BROWSER
    NAMES firefox firefox-x11 chrome chrominum
    )
if(BROWER_NOTFOUND)
    message(WARNING "Brower Not Found")
else()
    message(STATUS "Found Browser: ${BROWSER}")
endif()


# configurate documentation system.
# try to find Doxygen and Dot program.
include(FindDoxygen)
find_package(Doxygen
    QUIET
    COMPONENTS dot
    )
if(DOXYGEN_FOUND)
    message(STATUS "Doxygen Version: ${DOXYGEN_VERSION}")
endif()

# build document
set(DOXYGEN_EXCLUDE_PATTERNS _builds)
doxygen_add_docs(doc
    ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation"
    )

# open document
add_custom_target(open-doc
    COMMAND firefox ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM 
    )
add_dependencies(open-doc doc)

add_executable(noter main.cpp mainwindow.cpp mainwindow.h mainwindow.ui)
target_link_libraries(noter PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
