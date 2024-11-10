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

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace LX
{
    public partial class MSVC_VS_22_Compiler : MSVC_Compiler
    {

        // Function to compile a .cpp file to a .obj file
        public override bool CompileToObj(string fileName, out string error)
        {
            // Replace .cpp with .obj for the output file
            string objFileName = fileName.Replace(".cpp", ".obj");

            // Compiles the file
            using (Process p = Process.Start(generateStartUpInfo($"/EHsc /c \"{fileName}\" /Fo\"{objFileName}\" /link")))
            {
                if (p == null)
                {
                    error = "Failed to start process to compile .cpp into obj using MSVC";
                    return true;
                }

                // Waits for the Compilation to finish
                p.WaitForExit();

                if (p.ExitCode != 0)
                {
                    error = "Compilation from .cpp into obj, using MSVC, failed.";
                    return true;
                }
            }

            error = null;
            return false;
        }

        // Function to compile multiple .cpp files to multiple .obj files
        public override bool CompileToObjs(string[] fileNames, out string error)
        {
            // Loops through all the file names
            foreach (string fileName in fileNames)
            {
                if (CompileToObj(fileName, out error)) return true;
            }

            error = null;
            return false;
        }

        // Function to compile multiple .obj files to an .exe file
        public override bool LinkObjsToExe(string mainDir, string exeFileName, out string error)
        {
            // Creates the path to the .exe file
            DateTime now = DateTime.Now;
            exeFileName = Path.Combine(mainDir, exeFileName) + "_" + now.ToString("yyyy-MM-dd_HH-mm-ss") + ".exe";

            // Gets all the .obj files in the build directory
            string objFiles = string.Join(" ", System.IO.Directory.GetFiles(mainDir + "\\build", "*.obj"));

            // Links the .obj files to the .exe file
            using (Process p = Process.Start(generateStartUpInfo(objFiles + $" /link /OUT:\"{exeFileName}\"")))
            {
                // If the process failed to start, throw an exception
                if (p == null)
                {
                    error = "Failed to start MSVC linker.";
                    return true;
                }

                // Waits for the linking to finish
                p.WaitForExit();

                // If the process failed, throw an exception
                if (p.ExitCode != 0)
                {
                    error = "MSVC failed at linking the objs.\n" +
                            "Exit code: " + p.ExitCode 
                            + "\n" + p.StandardOutput.ReadToEnd();
                    return true;
                }
            }

            error = null;
            return false;
        }
    }
}
