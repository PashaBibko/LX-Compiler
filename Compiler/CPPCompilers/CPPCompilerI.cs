// Standard Libraries
using System;

// Project namespace
namespace LX
{
    // Base class for the compiler
    // All compilers will inherit from this class
    public interface CPPCompilerI
    {
        public string projectDir { get; set; }

        // Function to compile a .cpp file to a .obj file
        public bool CompileToObj(string fileName, out string error);

        // Function to compile multiple .cpp files to multiple .obj files
        public bool CompileToObjs(string[] fileNames, out string error);

        // Function to compile multiple .obj files to an .exe file
        public bool LinkObjsToExe(string mainDir, string exeFileName, out string error);
        /*{
            Console.WriteLine("Perhaps you should call the child class instead of the base class");
        }*/
    }
}
