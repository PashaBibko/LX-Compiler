// ======================================================================================= //
//                                                                                         //
// This code is license under a Proprietary License for LX - Compiler                      //
//                                                                                         //
// Copyright(c) 2024 Pasha Bibko                                                           //
//                                                                                         //
// 1. License Grant                                                                        //
//     You are granted a non - exclusive, non - transferable, and revocable                //
//     license to use this software for personal, educational, non - commercial,           //
//     or internal commercial purposes.You may install and use the software on             //
//     your devices or within your company, but you may not sell, sublicense,              //
//     or distribute the software in any form, either directly or as part                  //
//     of any derivative works. You may privately modify the software for                  //
//     internal use within your organization, provided that the modified versions          //
//     are not distributed, shared, or otherwise made available to third parties.          //
//                                                                                         //
// 2. Freedom to Share Creations                                                           //
//     You are free to create, modify, and share works or creations made with this         //
//     software, provided that you do not redistribute the original software itself.       //
//     All creations made with this software are solely your responsibility, and           //
//     you may license or distribute them as you wish, under your own terms.               //
//                                                                                         //
// 3. Restrictions                                                                         //
//     You may not:                                                                        //
//     - Sell, rent, lease, or distribute the original software or any copies              //
//       thereof, including modified versions.                                             //
//     - Distribute the software or modified versions to any third party.                  //
//                                                                                         //
// 4. Disclaimer of Warranty                                                               //
//     This software is provided "as is", without warranty of any kind, either             //
//     express or implied, including but not limited to the warranties of merchantability, //
//     fitness for a particular purpose, or non - infringement.In no event shall the       //
//     authors or copyright holders be liable for any claim, damages, or other liability,  //
//     whether in an action of contract, tort, or otherwise, arising from, out of, or in   //
//     connection with the software or the use or other dealings in the software.          //
//                                                                                         //
// ======================================================================================= //

// Standard Libraries
using System;
using System.Diagnostics;
using System.IO.Compression;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Text.Json;
using LX;

// Project namespace
namespace LX
{
    // Class to control the MSVC compiler
    public abstract class MSVC_Compiler : CPPCompilerI
    {
        public string projectDir { get; set; }

        // Path to the MSVC compiler
        protected string MSVCLocation;

        // Include and library paths for the MSVC compiler
        protected string MSVCIncludePath;
        protected string MSVCLibPath;

        // Include paths for the Windows SDK (Software Development Kit)
        protected string UCRTIncludePath;
        protected string sharedIncludePath;
        protected string UMIncludePath;

        // Library paths for the Windows SDK (Software Development Kit)
        protected string UCRTLibPath;
        protected string UMLibPath;

        protected ProcessStartInfo generateStartUpInfo(string extraArgs)
        {
            // Adds spaces to the beginning and end of the extra arguments
            // This is to make sure that the extra arguments are separated by spaces
            extraArgs = " " + extraArgs + " ";

            // Returns a ProcessStartInfo object with the following properties
            return new ProcessStartInfo
            {
                // Compiler location
                FileName = MSVCLocation,

                // Arguments
                Arguments =
                $"/I \"{MSVCIncludePath}\" /I \"{UCRTIncludePath}\" /I \"{sharedIncludePath}\" /I \"{UMIncludePath}\" /I \"{projectDir + "/build"}\"" // Include paths
                + extraArgs                                                                                                                           // Passed in arguments
                + $" /LIBPATH:\"{MSVCLibPath}\" /LIBPATH:\"{UCRTLibPath}\" /LIBPATH:\"{UMLibPath}\"",                                                 // Library paths

                // Default values
                UseShellExecute = false,
                RedirectStandardError = true,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            };
        }

        // Function to compile a .cpp file to a .obj file
        public abstract bool CompileToObj(string fileName, out string error);

        // Function to compile multiple .cpp files to multiple .obj files
        public abstract bool CompileToObjs(string[] fileNames, out string error);

        // Function to compile multiple .obj files to an .exe file
        public abstract bool LinkObjsToExe(string mainDir, string exeFileName, out string error);
    }
}
