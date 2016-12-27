genie clean
rmdir /s /q "../bin/Windows"
rmdir /s /q "../project/vs2015"
genie.exe --dotnet=msnet --file=premake_server.lua --os=windows --platform=x64 --to=../project/vs2015 vs2015
genie.exe --dotnet=msnet --file=premake_client.lua --os=windows --platform=x64 --to=../project/vs2015 vs2015
pause