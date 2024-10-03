// Standard Libraries
using System;
using System.Text.Json;
using System.Diagnostics;
using System.IO.Compression;
using System.Runtime.InteropServices;

// External Files
using LX_Compiler;
using System.Security.Cryptography.X509Certificates;

// Project namespace
namespace LX_Compiler
{
    // Class to control which compiler to use (and how to set it up)
    public class CompilerController
    {
        public static Exception InvalidComplierType(string type)
        {
            return new Exception
            (
                "Invalid Compiler Type: " + type + "\n" +
                "Valid types are:\n" +
                "\tMSVC-22"
            );
        }

        public static CompilerBase create(ref BuildInfo info)
        {
            // Gets the root element
            JsonElement root = info.jsonDoc.RootElement;

            // Gets the compiler JSON element
            JsonElement compilerJSON;
            try { compilerJSON = root.GetProperty("compiler"); }
            catch (KeyNotFoundException) { throw new Exception("Compiler not specified in build info"); }

            // Gets the compiler type
            string? compilerType;
            try { compilerType = compilerJSON.GetProperty("type").GetString(); }
            catch (KeyNotFoundException) { throw new Exception("Compiler Type not specified in build info"); }
            if (compilerType == null) { throw new Exception("Compiler Type not specified in build info"); }

            // Calls the relevant constuctor
            switch (compilerType)
            {
                case "MSVC-22": return new VS_22_Compiler(ref compilerJSON);

                // Throws an exception if the compiler type is invalid
                default: throw InvalidComplierType(compilerType);
            }

            throw new Exception("Compiler Type not specified in build info");
        }
    }

    // Main class
    public class Program
    {
        // Path to the C++ compiler DLL
        public const string DLL_CompilerLocation = "C:\\Users\\Pasha\\source\\github-repos\\LX-Compiler\\x64\\Debug\\Compiler.dll";

        // Includes the translate function from the DLL
        [DllImport(DLL_CompilerLocation, CallingConvention = CallingConvention.Cdecl)]
        private static extern void translate(string srcMainDir, string srcSubDir, string filename);

        // Main function
        static void Main(string[] args)
        {
            // Temporary code to allow it to be run from Visual Studio // REMOVE ON RELEASE
            if (args.Length == 0)
            {
                args = new string[1];
                args[0] = "C:\\Users\\Pasha\\source\\github-repos\\LX-Compiler\\test\\project.lx-build";
            }

            // Checks if it was called with the correct number of arguments
            if (args.Length != 1)
            {
                Console.WriteLine("Usage: LX_Compiler.exe <build-info-path>");
                Console.WriteLine("Only accepts one argument, received " + args.Length);
            }

            try
            {
                // Creates a new BuildInfo object
                // This object is used to store the build information
                BuildInfo info = new(args[0]);

                // Creates a new VS_Controller object
                CompilerBase c = CompilerController.create(ref info); //new VS_22_Compiler("C:\\Program Files\\Microsoft Visual Studio\\2022\\Community", "14.40.33807", "C:\\Program Files (x86)\\Windows Kits\\10", "10.0.22621.0");

                // Loops through all the source directories
                foreach (string srcDir in info.sourceDirs)
                {
                    // Finds all the .lx files in the source directory
                    string[] files = Directory.GetFiles(srcDir, "*.lx");

                    // Loops through all the .lx files
                    foreach (string file in files)
                    {
                        // Translates the .lx file to a .cpp file
                        translate(info.projectDir, System.IO.Path.GetFileNameWithoutExtension(srcDir), System.IO.Path.GetFileNameWithoutExtension(file) + ".lx");

                        // Compiles the .cpp file to a .obj file
                        c.CompileToObj(info.projectDir + "\\build\\" + System.IO.Path.GetFileNameWithoutExtension(file) + ".lx.cpp");
                    }
                }

                // Compiles the test.lx.obj file to a test.lx.exe file
                c.CompileObjsToExe(info.projectDir, info.projectName);
            }

            catch (Exception e)
            {
                Console.WriteLine("FATAL ERROR in: " + e.TargetSite);
                Console.WriteLine("ERROR: " + e.Message);
                Console.WriteLine();
                Console.WriteLine("Call Stack:");
                Console.WriteLine(e.StackTrace);
            }
        }
    }
}
