// Standard Libraries
using System;

// Project namespace
namespace LX_Compiler
{
    // Base class for the compiler
    // All compilers will inherit from this class
    public class CompilerBase
    {
        // Function to compile a .cpp file to a .obj file
        public virtual void CompileToObj(string fileName)
        {
            Console.WriteLine("Perhaps you should call the child class instead of the base class");
        }

        // Function to compile multiple .cpp files to multiple .obj files
        public virtual void CompileToObj(string[] fileNames)
        {
            Console.WriteLine("Perhaps you should call the child class instead of the base class");
        }

        // Function to compile multiple .obj files to an .exe file
        public virtual void CompileObjsToExe(string mainDir, string exeFileName)
        {
            Console.WriteLine("Perhaps you should call the child class instead of the base class");
        }
    }
}
