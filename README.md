# Methan0l  


## Documentation  

Available in the [Wiki Section](https://github.com/Hitonoriol/Methan0l/wiki)  


## How to use

**Execute a program**:  
```
./methan0l /path/to/src_file.mt0 [cmd_arg1 cmd_arg2 ...]
```  
\
**Start the interpreter in interactive mode:**  
```
./methan0l
```  


## Building Methan0l

**Build requirements:**  
* `g++10` or newer
* `libboost` (for `boost::dll` and its dependencies)  
* If you're using M$ Windows, you also need win32 ports of `gnu make` and `gnu coreutils` installed and added to `PATH` for build scripts to work ([MSYS2](https://msys2.org/) ports are a good choice)  

\
**Build interpreter:**  

```
cd build && make all
```  
\
**Build shared library & copy development headers:**  

```
cd build && make lib
```  

**Or for static library:**  
```
make static-lib
```  

As a result, `libmethan0l.so` or `libmethan0l.a` will be located in `build` directory and all headers will be copied under `build/methan0l-sdk/` preserving their directory structure.  

\
**Create an archive with Methan0l binary,  all modules, `libmethan0l.so` and development headers:**  

```
cd build && make release
```  

Resulting `methan0l-<OS_NAME><BITS>.tar.gz` will be located in `build` directory.  


## Tools

* [Methan0l IDE](https://github.com/Hitonoriol/Methan0l-IDE) - a mini IDE for Methan0l written in java.