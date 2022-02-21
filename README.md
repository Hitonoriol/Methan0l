# Methan0l

## Documentation
Available in the [Wiki Section](https://github.com/Hitonoriol/Methan0l/wiki)  

## How to use

### Build Methan0l

**Interpreter binary:**  

```
cd build && make all
```  
\
**Static & shared libraries & headers:**  

```
cd build && make lib
make static-lib
```  
As a result, `libmethan0l.so` & `libmethan0l.a` will be located under `build/methan0l-dev/` and all headers will be copied under `build/methan0l-sdk/include/` preserving their directory structure.  

\
**Full .tar archive with Methan0l binary,  all modules, `libmethan0l.so` and development headers:**  

```
cd build && make release
```  

Resulting `methan0l-<OS_NAME><BITS>.tar.gz` will be located in `build` directory.

\
**Build requirements:**  
* `g++10` or newer
* `libboost` (for `boost::dll` and its dependencies)  
* If you're using M$ Windows, you also need win32 ports of `gnu make` and `gnu coreutils` installed and added to `PATH` for build scripts to work

### Run Methan0l programs

**Execute** a program:  
```
./methan0l /path/to/src_file.mt0 [cmd_arg1 cmd_arg2 ...]
```  
\
**Interactive mode** (parse & execute expressions from stdin):  
```
./methan0l
```  

## Miscellaneous tools

* [Methan0l IDE](https://github.com/Hitonoriol/Methan0l-IDE) - a mini IDE written in java.