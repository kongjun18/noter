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

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

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
    option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})
    message(STATUS "Doxygen Version: ${DOXYGEN_VERSION}")
endif()

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    # install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)

    # open document
    add_custom_target(open-doc
        COMMAND firefox ${CMAKE_CURRENT_BINARY_DIR}/html/index.html
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM 
        )
    add_dependencies(open-doc doc)
endif()

set(RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.qrc)
qt_add_resources(RESOURCE_ADD ${RESOURCE})

set(SRC 
    main.cpp mainwindow.cpp 
    Editor.cpp
    StatusBarWidgets.cpp
    NotebookTreeWidget.cpp
    SearchPanel.cpp
    NoterConfig.cpp
    NotebookDialog.cpp
    NotebookListComboBox.cpp
    Highlighter.cpp)
set(HEAHDER
    mainwindow.h Editor.h log.h StatusBarWidgets.h  NotebookTreeWidget.h SearchPanel.h NoterConfig.h NotebookDialog.h NotebookTreeListComboBox.h Highlighter.h)
add_executable(noter ${RESOURCE_ADD} ${SRC} ${HEADER} mainwindow.ui)
target_link_libraries(noter PRIVATE Qt6::Widgets)
target_compile_options(noter PRIVATE $<$<CXX_COMPILER_ID:GNU>:-pedantic-errors -Wall -Wextra -Wsign-conversion -Wfloat-equal -Winline -Wunused>)
