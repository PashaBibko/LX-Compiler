// Standard Libraries
using System;
using System.Diagnostics;
using System.IO.Compression;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Text.Json;
using LX_Compiler;

// Project namespace
namespace LX_Compiler
{
    // Class to control the MSVC compiler
    public class VS_22_Compiler : CompilerBase
    {
        // Path to the MSVC compiler
        private readonly string MSVC_location;

        // Include and library paths for the MSVC compiler
        private readonly string msvcIncludePath;
        private readonly string msvcLibPath;

        // Include paths for the Windows SDK (Software Development Kit)
        private readonly string ucrtIncludePath;
        private readonly string sharedIncludePath;
        private readonly string umIncludePath;

        // Library paths for the Windows SDK (Software Development Kit)
        private readonly string ucrtLibPath;
        private readonly string umLibPath;

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
        public VS_22_Compiler(ref JsonElement compilerJSON)
        {
            // Gets the Visual Studio location
            string? VS_location;
            try { VS_location = compilerJSON.GetProperty("dir").GetString(); }
            catch (KeyNotFoundException) { throw new Exception("Visual Studio location not specified in build info. Should be under \"dir\""); }
            if (VS_location == null) { throw new Exception("Visual Studio location not specified in build info. Should be under \"dir\""); }

            // Gets the Visual Studio version
            string? VS_Version;
            try { VS_Version = compilerJSON.GetProperty("version").GetString(); }
            catch (KeyNotFoundException) { throw new Exception("Visual Studio version not specified in build info. Should be under \"version\""); }
            if (VS_Version == null) { throw new Exception("Visual Studio version not specified in build info. Should be under \"version\""); }

            // Gets the Windows SDK path
            string? windowsSdkPath;
            try { windowsSdkPath = compilerJSON.GetProperty("sdk-dir").GetString(); }
            catch (KeyNotFoundException) { throw new Exception("Windows SDK path not specified in build info. Should be under \"sdk-dir\""); }
            if (windowsSdkPath == null) { throw new Exception("Windows SDK path not specified in build info. Should be under \"sdk-dir\""); }

            // Gets the Windows SDK version
            string? windowsSDKVersion;
            try { windowsSDKVersion = compilerJSON.GetProperty("sdk-version").GetString(); }
            catch (KeyNotFoundException) { throw new Exception("Windows SDK version not specified in build info. Should be under \"sdk-version\""); }
            if (windowsSDKVersion == null) { throw new Exception("Windows SDK version not specified in build info. Should be under \"sdk-version\""); }

            // MSVC compiler location
            MSVC_location = System.IO.Path.Combine(VS_location, $"VC\\Tools\\MSVC\\{VS_Version}\\bin\\Hostx86\\x86");

            // MSVC C++ include and library paths
            msvcIncludePath = System.IO.Path.Combine(VS_location, $"VC\\Tools\\MSVC\\{VS_Version}\\include");
            msvcLibPath = System.IO.Path.Combine(VS_location, $"VC\\Tools\\MSVC\\{VS_Version}\\lib\\x86");

            // Checks if the paths are valid
            if (Directory.Exists(MSVC_location) == false) { throw new Exception("MSVC compiler location does not exist: " + MSVC_location); }
            if (Directory.Exists(msvcIncludePath) == false) { throw new Exception("MSVC include path does not exist: " + msvcIncludePath); }
            if (Directory.Exists(msvcLibPath) == false) { throw new Exception("MSVC library path does not exist: " + msvcLibPath); }

            // Updates the MSVC location to include the compiler
            MSVC_location = System.IO.Path.Combine(MSVC_location, "cl.exe");

            // Windows SDK include paths
            ucrtIncludePath = System.IO.Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\ucrt");
            sharedIncludePath = System.IO.Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\shared");
            umIncludePath = System.IO.Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\um");

            // Windows SDK library paths
            ucrtLibPath = System.IO.Path.Combine(windowsSdkPath, $"Lib\\{windowsSDKVersion}\\ucrt\\x86");
            umLibPath = System.IO.Path.Combine(windowsSdkPath, $"Lib\\{windowsSDKVersion}\\um\\x86");

            // Checks if the paths are valid
            if (Directory.Exists(ucrtIncludePath) == false) { throw new Exception("Windows SDK ucrt include path does not exist: " + ucrtIncludePath); }
            if (Directory.Exists(sharedIncludePath) == false) { throw new Exception("Windows SDK shared include path does not exist: " + sharedIncludePath); }
            if (Directory.Exists(umIncludePath) == false) { throw new Exception("Windows SDK um include path does not exist: " + umIncludePath); }
            if (Directory.Exists(ucrtLibPath) == false) { throw new Exception("Windows SDK ucrt library path does not exist: " + ucrtLibPath); }
            if (Directory.Exists(umLibPath) == false) { throw new Exception("Windows SDK um library path does not exist: " + umLibPath); }
        }

        // Function to compile a .cpp file to a .obj file
        public override void CompileToObj(string fileName)
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
            }
        }

        // Function to compile multiple .cpp files to multiple .obj files
        public override void CompileToObj(string[] fileNames)
        {
            // Loops through all the file names
            foreach (string fileName in fileNames) { CompileToObj(fileName); }
        }

        // Function to compile multiple .obj files to an .exe file
        public override void CompileObjsToExe(string mainDir, string exeFileName)
        {
            // Creates the path to the .exe file
            DateTime now = DateTime.Now;
            exeFileName = System.IO.Path.Combine(mainDir, exeFileName) + "[" + now.ToString("yyyy-MM-dd_HH-mm-ss") + "].exe";

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
            }
        }
    }
}
