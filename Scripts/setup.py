import os
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

import Vulkan
import Utils
import colorama
from colorama import Fore
from colorama import Back
from colorama import Style

colorama.init()

# Change from Scripts directory to root
os.chdir('../')

# Set ODYSSEY_ROOT environment variable to current Odyssey root directory
print(f"{Style.BRIGHT}{Back.GREEN}Setting ODYSSEY_ROOT to {os.getcwd()}{Style.RESET_ALL}")
subprocess.call(["setx", "ODYSSEY_ROOT", os.getcwd()])
os.environ['ODYSSEY_ROOT'] = os.getcwd()

if (not Vulkan.CheckVulkanSDK()):
    print("Vulkan SDK not installed.")
    exit()
    
if (Vulkan.CheckVulkanSDKDebugLibs()):
    print(f"{Style.BRIGHT}{Back.GREEN}Vulkan SDK debug libs located.{Style.RESET_ALL}")

subprocess.call(["git", "lfs", "pull"])
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

#if not os.path.exists("Hazelnut/DotNet/"):
    #os.makedirs("Hazelnut/DotNet/")

print(f"{Style.BRIGHT}{Back.GREEN}Generating Visual Studio 2022 solution.{Style.RESET_ALL}")
subprocess.call(["Scripts/premake5/premake5.exe", "vs2022"])

os.chdir('Projects/Sandbox')
subprocess.call(["../../Scripts/premake5/premake5.exe", "vs2022"])
