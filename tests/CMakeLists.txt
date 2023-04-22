# two_triangles
add_executable(two_triangles "${CMAKE_CURRENT_SOURCE_DIR}/two_triangles.cpp")
target_include_directories(two_triangles PRIVATE ${TANKGAME_EXT_INCLUDE})
target_link_libraries(two_triangles PRIVATE ${TANKGAME_EXT_LIB})

# camera
add_executable(empty_window "${CMAKE_CURRENT_SOURCE_DIR}/empty_window.cpp")
target_include_directories(empty_window PRIVATE ${TANKGAME_EXT_INCLUDE})
target_link_libraries(empty_window PRIVATE ${TANKGAME_EXT_LIB})