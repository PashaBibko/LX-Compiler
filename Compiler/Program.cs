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
        private static extern int lexSource(string folder, string srcDir, string fileName, bool debug);

        //
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool parseTokens(bool debug, int id);

        // 
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool translateAST(string folder, string filename, bool debug, int id);

        //
        [DllImport("API.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool CreateHeaderFile(string folder);

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

            // Foward declaration of the debug variable
            bool debug = false;

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
                        int ID = lexSource(info.ProjectDir, Path.GetFileNameWithoutExtension(srcDir), Path.GetFileNameWithoutExtension(file) + ".lx", debug);

                        if (ID == -1)
                        {
                            throw new Exception("An error occured during lexing");
                        }

                        if (parseTokens(debug, ID) == false)
                        {
                            throw new Exception("An error occured during parsing");
                        }
                    }

                    int nextID = 0;

                    foreach (string file in files)
                    {
                        if (translateAST(info.ProjectDir, Path.GetFileNameWithoutExtension(file) + ".lx", debug, nextID) == false)
                        {
                            throw new Exception("An error occured during translation");
                        }

                        nextID++;
                    }
                }

                // Creates a header file
                if (CreateHeaderFile(info.ProjectDir) == false)
                {
                    throw new Exception("An error occured during header file creation");
                }

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
                if (debug == true)
                {
                    Console.WriteLine("FATAL ERROR in: " + e.TargetSite);
                }

                // All neccessary info should be displayed here unless trying to debug the compiler itself
                Console.WriteLine("ERROR: " + e.Message);

                if (debug == true)
                {
                    Console.WriteLine();
                    Console.WriteLine("Call Stack:");
                    Console.WriteLine(e.StackTrace);
                }
            }
        }
    }
}
