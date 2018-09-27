add_library(text text.cpp)
add_library(word_processing word_processing.cpp)

add_executable(app main.cpp)
target_link_libraries(app text word_processing)
