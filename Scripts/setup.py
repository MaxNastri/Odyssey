import os
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

# Change from Scripts directory to root
os.chdir('../')

# Set ODYSSEY_ROOT environment variable to current Odyssey root directory
print(f"Setting ODYSSEY_ROOT to {os.getcwd()}")
subprocess.call(["setx", "ODYSSEY_ROOT", os.getcwd()])
os.environ['ODYSSEY_ROOT'] = os.getcwd()

print(f"Generating Visual Studio 2022 solution.")
subprocess.call(["scripts/premake5/premake5.exe", "vs2022"])

# os.chdir('Hazelnut/SandboxProject')
# subprocess.call(["../../vendor/bin/premake5.exe", "vs2022"])
os.system('pause')