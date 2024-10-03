// Standard Libraries
using System;
using System.Diagnostics;
using System.IO.Compression;
using System.Runtime.InteropServices;

// External Files
using LX_Compiler;

// Project namespace
namespace LX_Compiler
{
    // Main class
    public class Program
    {
        // Path to the C++ compiler DLL
        public const string DLL_CompilerLocation = "C:\\Users\\Pasha\\source\\github-repos\\LX-Compiler\\x64\\Debug\\Compiler.dll";

        // Includes the translate function from the DLL
        [DllImport(DLL_CompilerLocation, CallingConvention = CallingConvention.Cdecl)]
        private static extern void translate(string srcMainDir, string srcFileName, string outputDir);

        // Main function
        static void Main()
        {
            // Creates a new VS_Controller object
            VS_Controller vsController = new("C:\\Program Files\\Microsoft Visual Studio\\2022\\Community", "14.40.33807", "C:\\Program Files (x86)\\Windows Kits\\10", "10.0.22621.0");

            // String to the main directory
            string mainDir = "C:\\Users\\Pasha\\source\\github-repos\\LX-Compiler\\test";

            // Translates the test.lx file to a test.lx.cpp file
            translate(mainDir, "src", "test.lx");

            // Compiles the test.lx.cpp file to a test.lx.obj file
            vsController.CompileToObj(mainDir + "\\build\\test.lx.cpp");

            // Compiles the test.lx.obj file to a test.lx.exe file
            vsController.CompileObjsToExe(mainDir);
        }
    }
}
