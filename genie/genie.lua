-- Define the project. Put the release configuration first so it will be the
-- default when folks build using the makefile. That way they don't have to
-- worry about the /scripts argument and all that.
--

solution "multiplayer"
  configurations { "Release", "Debug" }
  location (_OPTIONS["to"])
  
  configuration "Debug"
    defines { "_DEBUG", "LUA_COMPAT_MODULE" }
    flags { "Symbols" }
    debugdir "../project/build/bin/debug"
    os.mkdir "../project/build/bin/debug"
    links {
      "openal32",
      "Box2D",
      "sfml-audio-d",
      "sfml-graphics-d",
      "sfml-network-d",
      "sfml-system-d",
      "sfml-window-d",
    }

  configuration "Release"
    defines { "NDEBUG", "LUA_COMPAT_MODULE" }
    flags { "Optimize" }
    debugdir "../project/build/bin/release"
    os.mkdir "../project/build/bin/release"
    links {
      "openal32",
      "Box2D",
      "sfml-graphics",
      "sfml-network",
      "sfml-system",
      "sfml-window",
    }

  configuration "vs*"
    defines { "_CRT_SECURE_NO_WARNINGS" }

  configuration "windows"
    targetdir "../project/build/bin/windows"

--
-- Use the --to=path option to control where the project files get generated. I use
-- this to create project files for each supported toolset, each in their own folder,
-- in preparation for deployment.
--
  newoption {
    trigger = "to",
    value   = "path",
    description = "Set the output location for the generated files"
  }
  
  os.mkdir("../project/build/bin/windows")
  os.copyfile("../lib/openal32.lib", "../project/build/bin/windows/openal32.lib")
  os.copyfile("../lib/openal32.dll", "../project/build/bin/windows/openal32.dll")
  os.copyfile("../lib/Box2D.lib", "../project/build/bin/windows/Box2D.lib")
  os.copyfile("../lib/sfml-audio-2.dll", "../project/build/bin/windows/sfml-audio-2.dll")
  os.copyfile("../lib/sfml-audio.lib", "../project/build/bin/windows/sfml-audio.lib")
  os.copyfile("../lib/sfml-audio-d-2.dll", "../project/build/bin/windows/sfml-audio-d-2.dll")
  os.copyfile("../lib/sfml-audio-d.lib", "../project/build/bin/windows/sfml-audio-d.lib")
  os.copyfile("../lib/sfml-graphics-2.dll", "../project/build/bin/windows/sfml-graphics-2.dll")
  os.copyfile("../lib/sfml-graphics.lib", "../project/build/bin/windows/sfml-graphics.lib")
  os.copyfile("../lib/sfml-graphics-d-2.dll", "../project/build/bin/windows/sfml-graphics-d-2.dll")
  os.copyfile("../lib/sfml-graphics-d.lib", "../project/build/bin/windows/sfml-graphics-d.lib")
  os.copyfile("../lib/sfml-network-2.dll", "../project/build/bin/windows/sfml-network-2.dll")
  os.copyfile("../lib/sfml-network.lib", "../project/build/bin/windows/sfml-network.lib")
  os.copyfile("../lib/sfml-network-d-2.dll", "../project/build/bin/windows/sfml-network-d-2.dll")
  os.copyfile("../lib/sfml-network-d.lib", "../project/build/bin/windows/sfml-network-d.lib")
  os.copyfile("../lib/sfml-system-2.dll", "../project/build/bin/windows/sfml-system-2.dll")
  os.copyfile("../lib/sfml-system.lib", "../project/build/bin/windows/sfml-system.lib")
  os.copyfile("../lib/sfml-system-d-2.dll", "../project/build/bin/windows/sfml-system-d-2.dll")
  os.copyfile("../lib/sfml-system-d.lib", "../project/build/bin/windows/sfml-system-d.lib")
  os.copyfile("../lib/sfml-window-2.dll", "../project/build/bin/windows/sfml-window-2.dll")
  os.copyfile("../lib/sfml-window.lib", "../project/build/bin/windows/sfml-window.lib")
  os.copyfile("../lib/sfml-window-d-2.dll", "../project/build/bin/windows/sfml-window-d-2.dll")
  os.copyfile("../lib/sfml-window-d.lib", "../project/build/bin/windows/sfml-window-d.lib")
  os.copyfile("../lib/System.Data.SQLite.dll", "../project/build/bin/windows/System.Data.SQLite.dll")

--[[--------------------------------------------
------------- MULTIPLAYER PROJECT --------------
--]]--------------------------------------------
project "multiplayer_client"
  targetname "multiplayer_client"
  language "C++"
  location "../project/build"
  libdirs "../lib"
  kind "ConsoleApp"
  
  includedirs {
    "../assets",
    "../include/client",
    "../deps/Box2D",
    "../deps/SFML/include",
  }
  
  files {
    "../src/client/**.*",
    "../include/client/**.*",
    "../deps/SFML/include/**.*",
    "../deps/Box2D/Box2D/Box2D.h",
  }
  
project "multiplayer_server"
  targetname "multiplayer_server"
  language "C#"
  location "../project/build"
  libdirs "../lib"
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