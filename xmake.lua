add_rules("mode.debug","mode.release")

add_includedirs("src/tools")
set_languages("c++23")

set_rundir("./")

target("dna")
    add_files("src/main.cpp")
