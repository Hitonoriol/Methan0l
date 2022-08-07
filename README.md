# Methan0l  


## Documentation  

Available in the [Wiki Section](https://github.com/Hitonoriol/Methan0l/wiki)  


## How to use

**Execute a program**:  
```
./methan0l [interpreter-args] /path/to/src-file.mt0 [script-args]
```  
\
**Start the interpreter in interactive mode:**  
```
./methan0l [interpreter-args]
```  

\
**Interpreter arguments:**  

* Set max allocatable heap capacity (in bytes):  
`--max-mem=123`
    
  
* Set initial heap capacity (in bytes):  
  `--initial-mem=123`  
  
* Allocate the whole heap fully (capacity is determined by the `--max-mem` value) on interpreter launch:  
  `--allocate-fully`

* Disable the heap limit. Effectively just stops the interpreter from throwing an exception on allocating more than max heap capacity:  
  `--no-heap-limit`

* Disable monotonic buffer upstream for heap allocations (for testing purposes, may get removed):  
  `--no-flat-buffer`

* Print version string:  
  `--version`


## Building Methan0l

**Build requirements:**  
* `GNU make`
* `g++10` or newer
* `libboost` (for `boost::dll` and its dependencies)  

For Windows machines you need to have ports of everything listed above installed and added to `PATH` ([MSYS2](https://msys2.org/) ports are recommended).  

\
**Build interpreter & CLI:**  

```
cd build && make all
```  
\
**Build interpreter only (`libmethan0l.so`):**  

```
cd build && make lib
```  
\
As a result, `libmethan0l.so` and / or CLI binary will be located in `build` directory and all headers will be copied under `build/include/` preserving their directory structure.  

\
**Build modules:**

```
cd build && make modules
```
or to build a specific module:
```
cd modules && make module-[module_name]
```
where `[module_name]` is the name of the directory that contains the module's sources.

\
**Create an archive with CLI binary, `libmethan0l.so`, all modules and development headers:**  

```
cd build && make release
```  

Resulting `methan0l-<OS_NAME><BITS>.tar.gz` will be located in `build` directory.  


## Tools

* [Methan0l IDE](https://github.com/Hitonoriol/Methan0l-IDE) - a mini IDE for Methan0l written in java.