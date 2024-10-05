// Standard Libraries
using System;
using System.IO;
using System.Runtime.CompilerServices;
using System.Text.Json;

// Project namespace
namespace LX
{
    // Class to store the build information
    // This is just used to store information for the selected compiler to use
    // Different compilers may have different build information that needs to be parsed separately (it will be stored in the same JSON but not in this object)
    public partial class BuildInfo
    {
        // Name of the project
        public string ProjectName { get; set; }
        public string ProjectDir { get; set; }

        // Currently just holds all the directories of the .lx source files
        public string[] SourceDirs { get; set; }

        // Holds the JSON object
        // Used by the compilers to get their respective build information
        public JsonDocument JsonDoc;

        public static bool Parse(string path, out BuildInfo res, out string error)
        {
            res = new BuildInfo();

            if (verifyBuildPath(path, out error)) return true;
            if (setProjDir(path, ref res, out error)) return true;
            res.ProjectName = Path.GetFileNameWithoutExtension(path);

            // Gets the source paths
            if (parseJSON(path, ref res, out error)) return true;

            return false;
        }

        BuildInfo()
        { }

        static bool verifyBuildPath(string buildInfoPath, out string error)
        {
            if (buildInfoPath == null)
            {
                error = "No build info path is provided (it is null).";
                return true;
            }

            if (File.Exists(buildInfoPath) == false)
            {
                error = $"The provided build info path ({buildInfoPath}) does not exist.";
                return true;
            }

            if (buildInfoPath.EndsWith(".lx-build") == false)
            {
                error = $"The provided build info path ({buildInfoPath}) is not a .lx-build file: ";
                return true;
            }

            error = null;
            return false;
        }

        static bool setProjDir(string buildInfoPath, ref BuildInfo res, out string error)
        {
            res.ProjectDir = Path.GetDirectoryName(buildInfoPath);

            if (res.ProjectDir == null)
            {
                error = "Project directory is null (IDK how u managed to get this error message). Good luck";
                return true;
            }

            error = null;
            return false;
        }
    }
}
