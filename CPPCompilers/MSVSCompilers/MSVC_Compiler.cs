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
                $"/I \"{MSVCIncludePath}\" /I \"{UCRTIncludePath}\" /I \"{sharedIncludePath}\" /I \"{UMIncludePath}\" "         // Include paths
                + extraArgs                                                                                                     // Passed in arguments
                + $" /LIBPATH:\"{MSVCLibPath}\" /LIBPATH:\"{UCRTLibPath}\" /LIBPATH:\"{UMLibPath}\"",                           // Library paths

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
