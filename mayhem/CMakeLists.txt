# Fuzz
add_executable(c-vector-fuzzer fuzzer.c)
target_link_libraries(c-vector-fuzzer c-vector)
target_compile_options(c-vector-fuzzer PUBLIC -fsanitize=fuzzer)
target_link_options(c-vector-fuzzer PUBLIC -fsanitize=fuzzer)

add_executable(c-vector-fuzzer-memory fuzzer.c)
target_link_libraries(c-vector-fuzzer-memory c-vector)
target_compile_options(c-vector-fuzzer-memory PUBLIC -fsanitize=fuzzer,memory)
target_link_options(c-vector-fuzzer-memory PUBLIC -fsanitize=fuzzer,memory)

add_executable(c-vector-fuzzer-address fuzzer.c)
target_link_libraries(c-vector-fuzzer-address c-vector)
target_compile_options(c-vector-fuzzer-address PUBLIC -fsanitize=fuzzer,address)
target_link_options(c-vector-fuzzer-address PUBLIC -fsanitize=fuzzer,address)
