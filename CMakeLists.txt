cmake_minimum_required(VERSION 3.14)

project(tank-game VERSION 0.01)

# ----- Options -----

option(TANKGAME_BUILD_TEST "if build tests" OFF)

# ----- External Libs -----

set(TANKGAME_EXT_MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/extern-module")

# --- opengl ---
find_package(OpenGL REQUIRED)

# --- assimp 5.0.0 ---
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
# exclude not needed importer
set(TANKGAME_ASSIMP_ALL_IMPORTERS
    AMF ASSBIN COLLADA CSM HMP OBJ OGRE OPENGEX IFC Q3D Q3BSP TERRAGEN GLTF MMD STEP
    3DS AC ASE B3D BVH DXF IRRMESH IRR LWO LWS MD2 MD3 MD5 MDC MDL NFF NDO OFF
    PLY MS3D COB BLEND XGL FBX RAW SIB SMD STL 3D X X3D 3MF)
set(TANKGAME_ASSIMP_RESERVE_IMPORTERS
    OBJ) # <-
list(REMOVE_ITEM
    TANKGAME_ASSIMP_ALL_IMPORTERS ${TANKGAME_ASSIMP_RESERVE_IMPORTERS})
foreach(NAME ${TANKGAME_ASSIMP_ALL_IMPORTERS})
    set("ASSIMP_BUILD_${NAME}_IMPORTER" CACHE BOOL OFF)
endforeach()
# exclude not needed exporters
set(TANKGAME_ASSIMP_ALL_EXPORTERS
    ASSXML COLLADA OBJ OPENGEX X3D GLTF ASSJSON 3DS ASSBIN PLY FBX STL X 3MF STEP)
set(TANKGAME_ASSIMP_RESERVE_EXPORTERS
    OBJ) # <-
list(REMOVE_ITEM
    TANKGAME_ASSIMP_ALL_EXPORTERS ${TANKGAME_ASSIMP_RESERVE_EXPORTERS})
message(${TANKGAME_ASSIMP_ALL_EXPORTERS})
foreach(NAME ${TANKGAME_ASSIMP_ALL_EXPORTERS})
    set("ASSIMP_BUILD_${NAME}_EXPORTER" CACHE BOOL OFF)
endforeach()

add_subdirectory("${TANKGAME_EXT_MOD_DIR}/assimp" EXCLUDE_FROM_ALL)

# --- glad ---
add_library(glad)
target_sources(glad PRIVATE "${TANKGAME_EXT_MOD_DIR}/glad/src/glad.c")
target_include_directories(glad PUBLIC "${TANKGAME_EXT_MOD_DIR}/glad/include")

# --- glfw 3.3 release ---
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory("${TANKGAME_EXT_MOD_DIR}/glfw")

# --- glm 0.9.9.6 release ---
# add_subdirectory cannot help find glm's header files
# so we do this ugly thing here
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${TANKGAME_EXT_MOD_DIR}/glm")

# --- learnopengl ---
add_library(learnopengl)
target_sources(learnopengl
    PRIVATE "${TANKGAME_EXT_MOD_DIR}/LearnOpenGL/src/model.cpp")
target_include_directories(learnopengl PUBLIC
    "${TANKGAME_EXT_MOD_DIR}/LearnOpenGL/includes")
target_link_libraries(learnopengl PRIVATE
        glm stb assimp glfw glad)

# --- plog 1.1.4 release ---
add_library(plog INTERFACE)
target_include_directories(plog INTERFACE "${TANKGAME_EXT_MOD_DIR}/plog/include")

# --- stb ---
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE "${TANKGAME_EXT_MOD_DIR}/stb")
unset(TANKGAME_EXT_MOD_DIR)

set(TANKGAME_EXT_LIB
    ${OPENGL_LIBRARIES} assimp glad glfw glm learnopengl stb plog)
set(TANKGAME_EXT_INCLUDE
    ${OPENGL_INCLUDE_DIR})

# ----- Build Tests -----

if (${TANKGAME_BUILD_TEST})
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
endif ()

# ----- My Libs -----

add_library(tankgamelib)
target_sources(tankgamelib PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/controller.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/display.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp")
target_include_directories(tankgamelib
    PRIVATE ${TANKGAME_EXT_INCLUDE}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(tankgamelib
    PRIVATE ${TANKGAME_EXT_LIB})
target_compile_features(tankgamelib PRIVATE cxx_std_17)

# ----- Build Executable -----

add_executable(tankgame)
target_sources(tankgame PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/app/main.cpp")
# include
target_include_directories(tankgame
    PRIVATE
        "${OPENGL_INCLUDE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/")
# libs
target_link_libraries(tankgame PRIVATE ${TANKGAME_EXT_LIB} tankgamelib)
target_compile_features(tankgame PRIVATE cxx_std_17)

if (MSVC)
    set_property(TARGET tankgame
        PROPERTY
            VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:tankgame>")
endif ()

# ----- Copy Resources -----

add_custom_command(TARGET tankgame POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resource"
        "$<TARGET_FILE_DIR:tankgame>/resource")
