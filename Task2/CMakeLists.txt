cmake_minimum_required (VERSION 3.15)

setupExecutableModule("Task2_client")
target_compile_options("Task2_client" PRIVATE "-DCLIENT")

setupExecutableModule("Task2_server")
target_compile_options("Task2_server" PRIVATE "-DSERVER")
