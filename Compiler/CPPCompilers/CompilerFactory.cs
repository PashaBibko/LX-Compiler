using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace LX
{
    public class CompilerFactory
    {
        public static Exception InvalidComplierType(string type)
        {
            return new Exception
            (

            );
        }

        public static bool Create(BuildInfo info, string buildInfoPath, out CPPCompilerI res, out string error)
        {
            // Gets the root element
            JsonElement root = info.JsonDoc.RootElement;

            // Gets the compiler JSON element
            JsonElement compilerJSON;
            try { compilerJSON = root.GetProperty("compiler"); }
            catch (KeyNotFoundException) { res = null; error = "Compiler not specified in build info (.lx-build file)"; return true; }

            // Gets the compiler type
            string compilerType;
            try { compilerType = compilerJSON.GetProperty("type").GetString(); }
            catch (KeyNotFoundException) { res = null; error = "Compiler Type not specified in build info (.lx-build file)"; return true; }
            if (compilerType == null) { res = null; error = "Compiler Type not specified in build info (.lx-build file)"; return true; }

            // Calls the relevant constuctor
            switch (compilerType)
            {
                case "MSVC-22":
                    MSVC_VS_22_Compiler tempRes;
                    if (MSVC_VS_22_Compiler.Create(buildInfoPath, ref compilerJSON, out tempRes, out error))
                    {
                        res = null;
                        return true;
                    }
                    res = tempRes;
                    break;
                default:
                    res = null;
                    error = "Invalid Compiler Type: " + compilerType + "\n" +
                            "Valid types are:\n" +
                            "\tMSVC-22";
                    return true;
            }

            return false;
        }
    }
}
