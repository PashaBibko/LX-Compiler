# LX-Compiler

This code is licensed under a propietary License found [here](LICENSE)

The official website for the LX language can be found [here WIP](https://www.youtube.com/watch?v=dQw4w9WgXcQ)

All information need to build and run the compiler is contained on this README but to use/learn the language it is recommended to use the offical website

## Build

This project requires you to be on Windows with Visual Studio 22 downloaded. We do plan to support additional Operating Systems and compilers in the future.

1. Download the project and run the .sln file
   
2. Run and build the project. It should compile but throw a runtime error
  
3. In the folder of /test create a file called version.json
 
4. In version.json create an object called "vs" and give it the value of your vs22 version. Typically found in "C:\\Program Files\\Microsoft Visual Studio\\2022\\Communityz\\VC\\Tools\\MSVC\\" (select highest version). If the path before is not valid you will have to locate your visual studio location and update the path in test/project.lx-build. The default path is: "C:\\Program Files\\Microsoft Visual Studio\\2022\\Community"

5. In version.json create an object called "sdk" and give it the value of your window kit version. Typically found in "C:\\Program Files (x86)\\Windows Kits\\10" (even on windows 10) and select the highest version. If the path before is not valid you will have to locate your window kits and update the path in test/project.lx-build. The default path is "C:\\Program Files (x86)\\Windows Kits\\10"

6. It should work. If it doesnt sucks to be you I guess
