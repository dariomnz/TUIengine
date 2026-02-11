
set(EXAMPLE_NAMES 
    "red-ball"
    "less"
    "draw"
)


foreach(EXAMPLE_NAME ${EXAMPLE_NAMES})
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.cpp)
    target_link_libraries(${EXAMPLE_NAME} TUIengine)
endforeach()