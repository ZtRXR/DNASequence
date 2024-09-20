add_rules("mode.debug","mode.release")
set_languages("c++23")

if is_mode("release")then
    set_optimize("aggressive") --这里使用了激进的优化，可能会导致浮点数计算不准确，考虑到本项目没有浮点计算，酌情考虑开启
    -- set_optimize("fastest") --上面和下面的二选一，--表示注释

    if is_plat("windows")then 
        -- 矢量化加速似乎没用
        -- add_cxxflags("/arch:AVX512") 
    elseif is_plat("linux") or is_plat("mingw") or is_plat("clang")then 
        -- 矢量化加速似乎没用
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
