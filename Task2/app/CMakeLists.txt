cmake_minimum_required (VERSION 3.15)

setupExecutableModule("Task2_app_client")
target_compile_options("Task2_app_client" PRIVATE "-DCLIENT")

setupExecutableModule("Task2_app_server")
target_compile_options("Task2_app_server" PRIVATE "-DSERVER")
