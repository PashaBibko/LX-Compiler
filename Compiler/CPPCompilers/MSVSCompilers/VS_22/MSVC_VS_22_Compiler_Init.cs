﻿// ======================================================================================= //
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
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace LX
{
    public partial class MSVC_VS_22_Compiler : MSVC_Compiler
    {
        static bool ReadFromJSON(string attribute, ref JsonElement compilerJSON, out string res)
        {
            try
            {
                res = compilerJSON.GetProperty(attribute).GetString();
            }
            catch (KeyNotFoundException)
            {
                res = null;
                return true;
            }

            if (res == null) return true;

            return false;
        }

        public static bool Create(string buildInfoPath, ref JsonElement compilerJSON, out MSVC_VS_22_Compiler res, out string error)
        {
            res = new MSVC_VS_22_Compiler();

            res.projectDir = Path.GetDirectoryName(buildInfoPath);

            // Gets the Visual Studio location
            string VS_Location;
            if (ReadFromJSON("dir", ref compilerJSON, out VS_Location))
            {
                error = "Visual Studio location not specified in build info (.lx-build file). Should be under \"dir\".";
                return true;
            }

            // Gets the Visual Studio version
            string VS_Version;
            if (ReadFromJSON("version", ref compilerJSON, out VS_Version))
            {
                error = "Visual Studio version not specified in build info (.lx-build file). Should be under \"version\".";
                return true;
            }

            // Gets the Windows SDK path
            string windowsSdkPath;
            if (ReadFromJSON("sdk-dir", ref compilerJSON, out windowsSdkPath))
            {
                error = "Windows SDK path not specified in build info (.lx-build file). Should be under \"sdk-dir\".";
                return true;
            }

            // Gets the Windows SDK version
            string windowsSDKVersion;
            if (ReadFromJSON("sdk-version", ref compilerJSON, out windowsSDKVersion))
            {
                error = "Windows SDK version not specified in build info (.lx-build file). Should be under \"sdk-version\".";
                return true;
            }

            // DEBUG CODE - REMOVE ON RELEASE
            if (VS_Version == "version.json" || windowsSDKVersion == "version.json")
            {
                // Path to the version.json file
                string versionJSONPath = Path.GetDirectoryName(buildInfoPath) + "\\version.json";

                // Opens the version.json file
                using (JsonDocument versionDoc = JsonDocument.Parse(File.ReadAllText(versionJSONPath)))
                {
                    // Gets the root element
                    JsonElement root = versionDoc.RootElement;

                    // Gets the Visual Studio version
                    if (VS_Version == "version.json")
                    {
                        try { VS_Version = root.GetProperty("vs").GetString(); }
                        catch (KeyNotFoundException) { throw new Exception("Visual Studio version not specified in version.json"); }
                        if (VS_Version == null) { throw new Exception("Visual Studio version not specified in version.json"); }
                    }

                    // Gets the Windows SDK version
                    if (windowsSDKVersion == "version.json")
                    {
                        try { windowsSDKVersion = root.GetProperty("sdk").GetString(); }
                        catch (KeyNotFoundException) { throw new Exception("Windows SDK version not specified in version.json"); }
                        if (windowsSDKVersion == null) { throw new Exception("Windows SDK version not specified in version.json"); }
                    }
                }
            }

            // MSVC compiler location
            res.MSVCLocation = Path.Combine(VS_Location, $"VC\\Tools\\MSVC\\{VS_Version}\\bin\\Hostx86\\x86\\cl.exe");

            // MSVC C++ include and library paths
            res.MSVCIncludePath = Path.Combine(VS_Location, $"VC\\Tools\\MSVC\\{VS_Version}\\include");
            res.MSVCLibPath = Path.Combine(VS_Location, $"VC\\Tools\\MSVC\\{VS_Version}\\lib\\x86");

            // Windows SDK include paths
            res.UCRTIncludePath = Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\ucrt");
            res.sharedIncludePath = Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\shared");
            res.UMIncludePath = Path.Combine(windowsSdkPath, $"Include\\{windowsSDKVersion}\\um");

            // Windows SDK library paths
            res.UCRTLibPath = Path.Combine(windowsSdkPath, $"Lib\\{windowsSDKVersion}\\ucrt\\x86");
            res.UMLibPath = Path.Combine(windowsSdkPath, $"Lib\\{windowsSDKVersion}\\um\\x86");

            if (ValidatePaths(ref res, out error)) return true;

            return false;
        }

        static bool ValidatePaths(ref MSVC_VS_22_Compiler res, out string error)
        {
            if (File.Exists(res.MSVCLocation) == false)
            {
                error = $"MSVC compiler location ({res.MSVCLocation}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.MSVCIncludePath) == false)
            {
                error = $"MSVC include path ({res.MSVCIncludePath}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.MSVCLibPath) == false)
            {
                error = $"MSVC library path ({res.MSVCLibPath}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.UCRTIncludePath) == false)
            {
                error = $"Windows SDK ucrt include path ({res.UCRTIncludePath}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.sharedIncludePath) == false)
            {
                error = $"Windows SDK shared include path ({res.sharedIncludePath}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.UMIncludePath) == false)
            {
                error = $"Windows SDK um include path ({res.UMIncludePath}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.UCRTLibPath) == false)
            {
                error = $"Windows SDK ucrt library path ({res.UCRTLibPath}) does not exist: ";
                return true;
            }
            if (Directory.Exists(res.UMLibPath) == false)
            {
                error = $"Windows SDK um library path ({res.UMLibPath}) does not exist: ";
                return true;
            }

            error = null;
            return false;
        }

        // Constructor to set all the needed paths
        MSVC_VS_22_Compiler()
        { }
    }
}
