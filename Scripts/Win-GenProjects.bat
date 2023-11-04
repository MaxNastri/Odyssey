@echo off
pushd ..
Scripts\premake5\premake5.exe vs2022
Scripts\premake5\premake5.exe vs2022 --file=premake5-native.lua
Scripts\premake5\premake5.exe vs2022 --file=premake5-managed.lua
dotnet build Odyssey-Managed.sln
popd
pause