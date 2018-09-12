add_library(solver solver.cpp)
add_executable(app main.cpp)
target_link_libraries(app solver)

add_executable(test_runner test.cpp)
target_link_libraries(test_runner solver)
