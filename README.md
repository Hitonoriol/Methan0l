
# Methan0l

## Documentation
Available in the [Wiki Section](https://github.com/Hitonoriol/Methan0l/wiki)  

## How to use

### Build Methan0l

**Interpreter binary:**  

```
cd build && make
```  
\
**Static library & headers:**  

```
cd build && make lib
```  
As a result, `libmethan0l.so` will be located under `build/methan0l-dev/` and all headers will be copied under `build/methan0l-dev/include/` preserving their directory structure.

\
**Dependencies:**  
* `g++10` (or any other compiler that supports C++17)
* `libboost` (for `boost::dll` and its dependencies)  

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