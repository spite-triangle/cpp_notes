add_rules("mode.debug")

target("demo")

    add_includedirs("src/","lib/")
    
    add_files("/src/*.cpp","lib/*.cpp")

    set_targetdir("bin")