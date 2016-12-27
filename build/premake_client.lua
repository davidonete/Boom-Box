solution "multiplayer_client"
	configurations { "Release", "Debug" }
	platforms { "x64", "Native", "Universal" }
	location (_OPTIONS["to"])

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }
		
	configuration {"windows", "x64"}
		targetdir "../bin/Windows"

	configuration "macosx"
		targetdir "../bin/MacOSX"

	newoption {
		trigger = "to",
		value   = "path",
		description = "Set the output location for the generated files"
	}
 
project "multiplayer_client"
	targetname "multiplayer_client"
	language "C++"
	location (_OPTIONS["to"] .. "/build")
	kind "ConsoleApp"
  	libdirs "../lib"
	
	links {
		"sfgui",
		"sfml-audio",
		"sfml-graphics",
		"sfml-network",
		"sfml-system",
		"sfml-window",
	}
  
	includedirs {
		"../assets",
		"../include/client",
		"../deps/SFML/include",
		"../deps/SFGUI/include",
		"../deps/Box2D"
	}
  
	files {
		"../src/client/**.*",
		"../include/client/**.*",
	}
		
	configuration "Debug"
		defines { "_DEBUG", "LUA_COMPAT_MODULE" }
		flags { "Symbols" }
		links {"Box2D-d"}
		
	configuration "Release"
		defines { "NDEBUG", "LUA_COMPAT_MODULE" }
		flags { "Optimize" }
		links {"Box2D"}
		
	configuration "windows" 
		os.mkdir("../bin/windows")
		os.copyfile("../lib/sfml-audio-2.dll", "../bin/Windows/sfml-audio-2.dll")
		os.copyfile("../lib/sfml-graphics-2.dll", "../bin/Windows/sfml-graphics-2.dll")
		os.copyfile("../lib/sfml-network-2.dll", "../bin/Windows/sfml-network-2.dll")
		os.copyfile("../lib/sfml-system-2.dll", "../bin/Windows/sfml-system-2.dll")
		os.copyfile("../lib/sfml-window-2.dll", "../bin/Windows/sfml-window-2.dll")
		
	-- TODO: MacOSX configuration