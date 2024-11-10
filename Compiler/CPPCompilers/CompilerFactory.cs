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
