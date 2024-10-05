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
        static bool parseJSON(string path, ref BuildInfo res, out string error)
        {
            if (getJSONDoc(path, out res.JsonDoc, out error)) return true;

            JsonElement root = res.JsonDoc.RootElement;

            // Create Source Directory Array from JSON
            try
            {
                var srcDirs = root.GetProperty("src-dir");
                res.SourceDirs = new string[srcDirs.GetArrayLength()];
            }
            catch (KeyNotFoundException)
            {
                error = "Source directory not specified in build info file (.lx-build).";
                return true;
            }

            // Get each src dir
            for (int i = 0; i < res.SourceDirs.Length; i++)
            {
                string dir = root.GetProperty("src-dir")[i].GetString();
                if (getSrcDir(dir, ref res, out res.SourceDirs[i], out error)) return true;
            }

            return false;
        }

        static bool getJSONDoc(string path, out JsonDocument res, out string error)
        {
            try
            {
                var text = File.ReadAllText(path);
                res = JsonDocument.Parse(text);
            }
            catch (Exception e)
            {
                res = null;
                error = "Could not parse .lx-build's JSON format: " + e.Message;
                return true;
            }

            error = null;
            return false;
        }

        static bool getSrcDir(string srcDir, ref BuildInfo info, out string res, out string error)
        {
            if (srcDir == null)
            {
                res = null;
                error = "A source dir provided in the .lx-build file is null.";
                return true;
            }

            // Combines the project directory with the source path to get the full path
            res = Path.Combine(info.ProjectDir, srcDir);

            // Checks that the source path is a valid directory
            if (!Directory.Exists(res))
            {
                error = $"A source dir ({res}) provided in the .lx-build file does not exist";
                return true;
            }

            error = null;
            return false;
        }
    }
}
