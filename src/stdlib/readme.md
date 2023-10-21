# Methan0l Standard Library

Files in this folder are not part of the interpreter core.  
Each `Lib*.cpp` file is compiled and linked into its own shared library, which is then placed into the interpreter library folder.  


### Dependencies

When a library needs to be linked with any external dependencies, a `Lib*.deps` file should be used.