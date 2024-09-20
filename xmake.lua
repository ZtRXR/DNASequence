add_rules("mode.debug","mode.release")
set_languages("c++23")

if is_mode("release")then
    set_optimize("aggressive") --这里使用了激进的优化，可能会导致浮点数计算不准确，考虑到本项目没有浮点计算，酌情考虑开启
    -- set_optimize("fastest") --上面和下面的二选一，--表示注释

    if is_plat("windows")then 
        -- 注意，下面是启用AVX512指令集矢量化加速，只有新2010年后的CPU支持，但是可以大大加快批量指令加速，可以试试将下面的--删除
        -- 但是似乎这个程序目前任然是IO密集型，性能差不多，目前注释掉了
        -- add_cxxflags("/arch:AVX512") 
    elseif is_plat("linux") or is_plat("mingw") or is_plat("clang")then 
        -- 启用AVX512指令集矢量化加速，可能会导致无法在除了编译本程序的电脑上运行，出现兼容问题
        -- add_cxxflags("-march=native")
    end 
end

if is_plat("windows")then 
    add_cxxflags("/openmp")
elseif is_plat("linux") or is_plat("mingw") or is_plat("clang")then 
    add_cxxflags("-fopenmp")
    add_ldflags("-fopenmp")
end 

add_includedirs("src/tools")
set_rundir("./")

target("dna")
    add_files("src/main.cpp")
