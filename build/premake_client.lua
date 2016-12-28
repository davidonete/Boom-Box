solution "multiplayer_client"
	configurations { "Release", "Debug" }
	platforms { "x64", "Native", "Universal" }
	location (_OPTIONS["to"])

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }

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
		libdirs "../lib/Debug"
		defines { "_DEBUG", "LUA_COMPAT_MODULE" }
		flags { "Symbols" }
		links {
			"Box2D-d",
			"sfgui-d",
			"sfml-audio-d",
			"sfml-graphics-d",
			"sfml-network-d",
			"sfml-system-d",
			"sfml-window-d"
		}
		
	configuration "Release"
		libdirs "../lib/Release"
		defines { "NDEBUG", "LUA_COMPAT_MODULE" }
		flags { "Optimize" }
		links {
			"Box2D",
			"sfgui",
			"sfml-audio",
			"sfml-graphics",
			"sfml-network",
			"sfml-system",
			"sfml-window",
		}
		
	configuration {"windows", "Debug"}
		targetdir "../bin/Windows/Debug"
		debugdir "../bin/Windows/Debug"
		
	configuration {"windows", "Release"}
		targetdir "../bin/Windows/Release"
		debugdir "../bin/Windows/Release"
		
	configuration "windows" 
		os.mkdir("../bin/Windows/Release")
		os.copyfile("../lib/Release/sfgui.dll", "../bin/Windows/Release/sfgui.dll")
		os.copyfile("../lib/Release/sfml-audio-2.dll", "../bin/Windows/Release/sfml-audio-2.dll")
		os.copyfile("../lib/Release/sfml-graphics-2.dll", "../bin/Windows/Release/sfml-graphics-2.dll")
		os.copyfile("../lib/Release/sfml-network-2.dll", "../bin/Windows/Release/sfml-network-2.dll")
		os.copyfile("../lib/Release/sfml-system-2.dll", "../bin/Windows/Release/sfml-system-2.dll")
		os.copyfile("../lib/Release/sfml-window-2.dll", "../bin/Windows/Release/sfml-window-2.dll")
		os.mkdir("../bin/Windows/Debug")
		os.copyfile("../lib/Debug/sfgui-d.dll", "../bin/Windows/Debug/sfgui-d.dll")
		os.copyfile("../lib/Debug/sfml-audio-d-2.dll", "../bin/Windows/Debug/sfml-audio-d-2.dll")
		os.copyfile("../lib/Debug/sfml-graphics-d-2.dll", "../bin/Windows/Debug/sfml-graphics-d-2.dll")
		os.copyfile("../lib/Debug/sfml-network-d-2.dll", "../bin/Windows/Debug/sfml-network-d-2.dll")
		os.copyfile("../lib/Debug/sfml-system-d-2.dll", "../bin/Windows/Debug/sfml-system-d-2.dll")
		os.copyfile("../lib/Debug/sfml-window-d-2.dll", "../bin/Windows/Debug/sfml-window-d-2.dll")
		
	configuration "macosx"
		targetdir "../bin/MacOSX/Release"
		debugdir "../bin/MacOSX/Debug"
		
	-- TODO: MacOSX configuration