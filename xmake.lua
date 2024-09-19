add_rules("mode.debug","mode.release")

add_includedirs("src/tools")
set_languages("c++23")

set_rundir("./")

if is_plat("windows")then 
    -- 注意，这里是启用AVX512指令集矢量化加速，只有新2010年后的CPU支持，但是可以大大加快批量指令加速，可以试试将下面的--删除
    -- 但是似乎这个程序目前任然是IO密集型，性能差不多
    -- add_cxxflags("/arch:AVX512") 
elseif is_plat("linux") or is_plat("mingw") or is_plat("clang")then 
    -- 启用AVX512指令集矢量化加速，可能会导致无法在除了编译本程序的电脑上运行，出现兼容问题
    -- add_cxxflags("-march=native")
end 

target("dna")
    add_files("src/main.cpp")
