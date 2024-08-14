@echo off
pushd %~dp0
..\..\Scripts\premake5\premake5.exe vs2022 --file=premake5.lua
popd