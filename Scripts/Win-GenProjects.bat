@echo off
pushd ..
Scripts\premake5\premake5.exe vs2022 --file=premake5.lua
popd
pause