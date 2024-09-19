add_rules("mode.debug","mode.release")

add_includedirs("src/tools")
set_languages("c++23")

set_rundir("./")

if is_plat("windows")then 
    -- 注意，这里是启用AVX512指令集矢量化加速，只有新2010年后的CPU支持，但是可以大大加快批量指令加速
    -- add_cxxflags("/arch:AVX512") 
end

target("dna")
    add_files("src/main.cpp")
