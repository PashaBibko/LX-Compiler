﻿// Standard Libraries
using System;
using System.Text.Json;
using System.Diagnostics;
using System.IO.Compression;
using System.Runtime.InteropServices;

// External Files
using LX;
using System.Security.Cryptography.X509Certificates;

// Project namespace
namespace LX
{
    // Main class
    public class Program
    {
        // Includes the translate function from the DLL
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void lexSource(string folder, string srcDir, string fileName, bool debug);

        //
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void parseTokens();

        // 
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void translateAST(string folder, string filename);

        //
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void CreateHeaderFile(string folder);

        // Main function
        static void Main(string[] args)
        {
            // Temporary code to allow it to be run from Visual Studio // REMOVE ON RELEASE
            if (args.Length == 0)
            {
                args = new string[1];

                // Move up to the solution directory
                #pragma warning disable CS8602 // Dereference of a possibly null reference.
                string solutionDir = Directory.GetParent(AppDomain.CurrentDomain.BaseDirectory).Parent.Parent.Parent.Parent.FullName;
                #pragma warning restore CS8602 // Dereference of a possibly null reference.

                args[0] = solutionDir + "\\example\\project.lx-build";
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
                BuildInfo info;
                string error;
                if (BuildInfo.Parse(args[0], out info, out error))
                {
                    Console.WriteLine("An error ocurred whilst parsing the .lx-build file: ");
                    Console.WriteLine(error);
                    return;
                }

                // Creates a new VS_Controller object
                CPPCompilerI c;
                if (CompilerFactory.Create(info, args[0], out c, out error))
                {
                    Console.WriteLine("An error ocurred whilst initializing the compiler: ");
                    Console.WriteLine(error);
                    return;
                }

                // Determines if the program is in debug mode
                bool debug = false;
                try { debug = info.JsonDoc.RootElement.GetProperty("debug").GetBoolean(); }
                catch (KeyNotFoundException) { /* Should be empty */ }

                // Loops through all the source directories
                foreach (string srcDir in info.SourceDirs)
                {
                    // Finds all the .lx files in the source directory
                    string[] files = Directory.GetFiles(srcDir, "*.lx");

                    // Loops through all the .lx files
                    foreach (string file in files)
                    {
                        lexSource(info.ProjectDir, Path.GetFileNameWithoutExtension(srcDir), Path.GetFileNameWithoutExtension(file) + ".lx", debug);
                        parseTokens();
                        translateAST(info.ProjectDir, Path.GetFileNameWithoutExtension(file) + ".lx");
                    }
                }

                // Creates a header file
                CreateHeaderFile(info.ProjectDir);

                // Loop through all the .cpp files in the project directory
                foreach (string file in Directory.GetFiles(Path.Combine(info.ProjectDir, "build"), "*.cpp"))
                {
                    // Compiles the .cpp file to a .obj file
                    if (c.CompileToObj(file, out error))
                    {
                        Console.WriteLine($"An error occured during compilation: ");
                        Console.WriteLine(error);
                        return;
                    }
                }

                // Compiles the test.lx.obj file to a test.lx.exe file
                if (c.LinkObjsToExe(info.ProjectDir, info.ProjectName, out error))
                {
                    Console.WriteLine($"An error occured during linking: ");
                    Console.WriteLine(error);
                    return;
                }
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
