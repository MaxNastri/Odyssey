# Odyssey
Odyssey Engine development repository.

![image](https://github.com/user-attachments/assets/315fa27c-7ce5-4c34-a17b-95aec2fa1792)

## Features
* Vulkan rendering
* GPU-based particle rendering
* Skeletal Animation
* Realtime shadowmapping
* Editor
* C# Scripting
* Entity Component System

## Supported Platforms
Currently Odyssey is only supported on windows and uses a Visual Studio 2022 build system via Premake5.

## Build Instructions
1. Run the Setup.bat file located in the Scripts folder to install any missing dependencies and generate the Visual Studio 2022 solution files.
2. Open the Odyssey solution file located in the repository root folder.
3. Build the solution in Release mode.

## Navigating the Editor
* The editor default opens the Sandbox project.
* The content browser window displays the files located within the Sandbox project including assets, scripts, etc.
* Clicking an object in the Scene Hierarchy window will show it's components in the Inspector window.
* If the object has a Transform component, the transformation gizmo will appear in the Scene View window.
