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
                    error = "MSVC failed at linking the objs.";
                    return true;
                }
            }

            error = null;
            return false;
        }
    }
}
