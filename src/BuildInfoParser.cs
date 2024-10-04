// Standard Libraries
using System;
using System.Runtime.CompilerServices;
using System.Text.Json;

// Project namespace
namespace LX_Compiler
{
    // Class to store the build information
    // This is just used to store information for the selected compiler to use
    // Different compilers may have different build information that needs to be parsed separately (it will be stored in the same JSON but not in this object)
    public class BuildInfo
    {
        // Name of the project
        public string projectName;
        public string projectDir;

        // Currently just holds all the directories of the .lx source files
        public string[] sourceDirs;

        // Holds the JSON object
        // Used by the compilers to get their respective build information
        public JsonDocument jsonDoc;

        // Constructor to set the source paths
        public BuildInfo(string buildInfoPath)
        {
            // Verifies the build info path
            verifyBuildPath(buildInfoPath);

            // Gets the project directory
            string? projectDir_n = System.IO.Path.GetDirectoryName(buildInfoPath);
            if (projectDir_n == null) { throw new Exception("Project directory is null (IDK how u managed to get this error message). Good luck"); }
            else { projectDir = projectDir_n; }

            // Gets the project name
            projectName = System.IO.Path.GetFileNameWithoutExtension(buildInfoPath);

            // Parses the JSON file
            jsonDoc = JsonDocument.Parse(File.ReadAllText(buildInfoPath));

            // Gets the root element
            JsonElement root = jsonDoc.RootElement;

            // Gets the source paths
            try { sourceDirs = new string[root.GetProperty("src-dir").GetArrayLength()]; }
            catch (KeyNotFoundException) { throw new Exception("Source path not specified in build info"); }

            // Loops through the source paths
            for (int i = 0; i < sourceDirs.Length; i++)
            {
                // Gets the source path from the JSON file
                string? newPath = root.GetProperty("src-dir")[i].GetString();

                // Checks if the source path is null
                if (newPath == null)
                {
                    // Throws an exception
                    throw new Exception("Source path is null");
                }

                // Combines the project directory with the source path to get the full path
                // This makes it easier to work with the source paths (and C# functions)
                newPath = System.IO.Path.Combine(projectDir, newPath);

                // Checks that the source path is a valid directory
                if (Directory.Exists(newPath) == false)
                {
                    // Throws an exception
                    throw new Exception("Source path does not exist: " + newPath);
                }

                // If the source path is valid, sets the source path
                sourceDirs[i] = newPath;
            }
        }

        private static void verifyBuildPath(string buildInfoPath)
        {
            // Checks if the build info path is null else throws an exception
            if (buildInfoPath == null)                         { throw new Exception("Build info path is null: " + buildInfoPath);                 }

            // Checks if the build info path is a valid file else throws an exception
            if (System.IO.File.Exists(buildInfoPath) == false) { throw new Exception("Build info path does not exist: " + buildInfoPath);          }

            // Checks if the build info path is a LX-Build file else throws an exception
            if (buildInfoPath.EndsWith(".lx-build") == false)  { throw new Exception("Build info path is not a .lx-build file: " + buildInfoPath); }
        }
    }
}
