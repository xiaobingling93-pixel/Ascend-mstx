# 安全规范编译命令
add_compile_options("-Wall")
add_compile_options("-fPIC")
add_compile_options("-fstack-protector-all")
add_compile_options("-D_FORTIFY_SOURCE=2")
add_compile_options("-fvisibility=hidden")
add_compile_options("-ftrapv")
add_compile_options("-fstack-check")

add_link_options("-Wl,-z,relro")
add_link_options("-Wl,-z,now")
add_link_options("-Wl,-z,noexecstack")
add_link_options("-pie")
add_link_options("-s") #strip