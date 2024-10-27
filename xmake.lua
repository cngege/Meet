-- define target

add_rules("mode.debug", "mode.release")
target("Meet")
	
	add_ldflags("-Wl,-exec-charset=UTF-8")
    -- set kind
    set_kind("binary")
	set_languages("c++20")
	
	
    -- add files
    add_files("Meet/*.cpp")
