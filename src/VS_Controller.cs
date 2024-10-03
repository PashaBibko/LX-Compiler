// Standard Libraries
using System;
using System.Diagnostics;
using System.IO.Compression;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;

// Project namespace
namespace LX_Compiler
{
    // Class to control the MSVC compiler
    public class VS_Controller
    {
        // Path to the MSVC compiler
        string MSVC_location;

        // Include and library paths for the MSVC compiler
        string msvcIncludePath;
        string msvcLibPath;

        // Include paths for the Windows SDK (Software Development Kit)
        string ucrtIncludePath;
        string sharedIncludePath;
        string umIncludePath;

        // Library paths for the Windows SDK (Software Development Kit)
        string ucrtLibPath;
        string umLibPath;

        // Function to make the generation of the ProcessStartInfo object easier
        private ProcessStartInfo generateStartUpInfo(string extraArgs)
        {
            // Adds spaces to the beginning and end of the extra arguments
            // This is to make sure that the extra arguments are separated by spaces
            extraArgs = " " + extraArgs + " ";

            // Returns a ProcessStartInfo object with the following properties
            return new ProcessStartInfo
            {
                // Compiler location
                FileName = MSVC_location,

                // Arguments
                Arguments =
                $"/I \"{msvcIncludePath}\" /I \"{ucrtIncludePath}\" /I \"{sharedIncludePath}\" /I \"{umIncludePath}\" "         // Include paths
                + extraArgs                                                                                                     // Passed in arguments
                + $" /LIBPATH:\"{msvcLibPath}\" /LIBPATH:\"{ucrtLibPath}\" /LIBPATH:\"{umLibPath}\"",                           // Library paths

                // Default values
                UseShellExecute = false,
                RedirectStandardError = true,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            };
        }

        // Constructor to set all the needed paths
        public VS_Controller(string VS_location, string VS_Version, string windowsSdkPath, string windowsSDKVersion)
        {
            // MSVC compiler location
            MSVC_location = System.IO.Path.Combine(VS_location, $"VC\\Tools\\MSVC\\{VS_Version}\\bin\\Hostx86\\x86\\cl.exe");

            // MSVC C++ include and library paths
            msvcIncludePath = System.IO.Path.Combine(VS_location, $"VC\\Tools\\MSVC\\{VS_Version}\\include");
            msvcLibPath = System.IO.Path.Combine(VS_location, $"VC\\Tools\\MSVC\\{VS_Version}\\lib\\x86");

            // Windows SDK include paths
            ucrtIncludePath = System.IO.Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\ucrt");
            sharedIncludePath = System.IO.Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\shared");
            umIncludePath = System.IO.Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\um");

            // Windows SDK library paths
            ucrtLibPath = System.IO.Path.Combine(windowsSdkPath, $"Lib\\{windowsSDKVersion}\\ucrt\\x86");
            umLibPath = System.IO.Path.Combine(windowsSdkPath, $"Lib\\{windowsSDKVersion}\\um\\x86");
        }

        // Function to compile a .cpp file to a .obj file
        public void CompileToObj(string fileName)
        {
            // Replace .cpp with .obj for the output file
            string objFileName = fileName.Replace(".cpp", ".obj");

            // Compiles the file
            using (Process? p = Process.Start(generateStartUpInfo($"/EHsc /c \"{fileName}\" /Fo\"{objFileName}\" /link")))
            {
                // If the process failed to start, throw an exception
                if (p == null) { throw new Exception("Failed to start process"); }

                // Waits for the compiling to finish
                p.WaitForExit();

                // If the process failed, throw an exception
                if (p.ExitCode != 0) { throw new Exception("Compilation failed"); }

                // Writes to the console that the file was compiled
                Console.WriteLine($"Compiled {fileName} to {objFileName}");
            }
        }

        // Function to compile multiple .cpp files to multiple .obj files
        public void CompileToObj(string[] fileNames)
        {
            // Loops through all the file names
            foreach (string fileName in fileNames) { CompileToObj(fileName); }
        }

        // Function to compile multiple .obj files to an .exe file
        public void CompileObjsToExe(string mainDir)
        {
            // Creates the path to the .exe file
            string exeFileName = mainDir + "\\test.exe";

            // Gets all the .obj files in the build directory
            string objFiles = string.Join(" ", System.IO.Directory.GetFiles(mainDir + "\\build", "*.obj"));

            // Links the .obj files to the .exe file
            using (Process? p = Process.Start(generateStartUpInfo(objFiles + $" /link /OUT:\"{exeFileName}\"")))
            {
                // If the process failed to start, throw an exception
                if (p == null) { throw new Exception("Failed to start process"); }

                // Waits for the linking to finish
                p.WaitForExit();

                // If the process failed, throw an exception
                if (p.ExitCode != 0) { throw new Exception("Linking failed"); }

                // Writes to the console that the files were linked
                Console.WriteLine($"Linked {objFiles} to {exeFileName}");
            }
        }
    }
}
