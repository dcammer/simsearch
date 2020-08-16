add_executable(test_myminhash catch_main.cpp test_minhash.cpp)
target_include_directories(test_myminhash PUBLIC ../include)
target_link_libraries(test_myminhash fmt::fmt Catch2::Catch2)
