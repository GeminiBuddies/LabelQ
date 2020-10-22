cmake_minimum_required(VERSION 3.12)

project(labelq LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# prepend and append functions
function(prepend var list prefix)
    set(rv "")
    foreach(i ${list})
        list(APPEND rv "${prefix}${i}")
    endforeach(i)
    set(${var} "${rv}" PARENT_SCOPE)
endfunction(prepend)

function(append var list suffix)
    set(rv "")
    foreach(i ${list})
        list(APPEND rv "${i}${suffix}")
    endforeach(i)
    set(${var} "${rv}" PARENT_SCOPE)
endfunction(append)


# load qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets LinguistTools REQUIRED)

# language list
set (LANGUAGES
    en_US
    zh_CN
)

append(INTERMEDIATE_TS_FILEPATH "${LANGUAGES}" ".ts")
prepend(INTERMEDIATE_TS_FILEPATH "${INTERMEDIATE_TS_FILEPATH}" "${CMAKE_CURRENT_BINARY_DIR}/")
append(FINAL_TS_FILEPATH "${LANGUAGES}" ".ts")
prepend(FINAL_TS_FILEPATH "${FINAL_TS_FILEPATH}" "${CMAKE_CURRENT_BINARY_DIR}/labelq_")

# ts_builder
add_executable(ts_builder ts_builder_src/ts_builder.cpp)

add_custom_command(
    OUTPUT ${FINAL_TS_FILEPATH}
    COMMAND ts_builder -i ${INTERMEDIATE_TS_FILEPATH} -o ${FINAL_TS_FILEPATH}
    DEPENDS ts_builder ${INTERMEDIATE_TS_FILEPATH} "${CMAKE_CURRENT_LIST_DIR}/languages/"
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/languages/"
)

# qt linguist call
if (WIN32)
  set(GUI_EXEC_WIN32 WIN32)
endif()

qt5_create_translation(QM_FILES_INTERMEDIATE ${CMAKE_SOURCE_DIR} ${INTERMEDIATE_TS_FILEPATH})
qt5_add_translation(QM_FILES ${FINAL_TS_FILEPATH})

# make labelq
file(GLOB_RECURSE SRC "src/*.cpp")
file(GLOB HEADERS "include/*.h")
file(GLOB UIS "ui/*.ui")

add_executable(labelq ${GUI_EXEC_WIN32}
    main.cpp
    labelq.qrc
    ${SRC}
    ${HEADERS}
    ${UIS}
    ${QM_FILES}
)

target_include_directories(labelq PRIVATE ./include)

target_link_libraries(labelq PRIVATE Qt5::Widgets)
