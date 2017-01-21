solution "multiplayer_server"
	configurations { "Release", "Debug" }
	platforms { "x64", "Native", "Universal" }
	location (_OPTIONS["to"])

	configuration "vs*"
		defines { "_CRT_SECURE_NO_WARNINGS" }
		
	configuration {"windows", "x64"}
		targetdir "../bin/Windows/Release"
		debugdir "../bin/Windows/Release"

	configuration "macosx"
		targetdir "../bin/MacOSX"

	newoption {
		trigger = "to",
		value   = "path",
		description = "Set the output location for the generated files"
	}
		
project "multiplayer_server"
	targetname "multiplayer_server"
	language "C#"
	location (_OPTIONS["to"] .. "/build")
	kind "ConsoleApp"

	links {
		"Microsoft.CSharp", 
		"System",
		"System.Data",
		"System.Data.DataSetExtensions",
		"System.Net.Http",
		"System.XML",
		"System.Xml.Linq",
		"System.Data.SQLite"
	}

	includedirs {
		"../include/server",
	}

	files {
		"../src/server/**.*",
		"../include/server/**.*",
	}
	
	configuration "Debug"
		defines { "_DEBUG", "LUA_COMPAT_MODULE" }
		flags { "Symbols" }
		
	configuration "Release"
		defines { "NDEBUG", "LUA_COMPAT_MODULE" }
		flags { "Optimize" }
		
	configuration "windows" 
		os.mkdir("../bin/Windows/Release")
		os.copyfile("../lib/Release/System.Data.SQLite.dll", "../bin/Windows/Release/System.Data.SQLite.dll")
		os.copyfile("../data/database/database_win.db", "../bin/Windows/Release/database.db")
	--TODO: MacOSX configuration