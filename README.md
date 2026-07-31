# Methan0l  

Embeddable AST (tree-walking) interpreter for Methan0l, a dynamically-typed multi-paradigm interpreted programming language.  

## Language Reference Documentation  

Available [here](https://methan0l.github.io/docs/).  


## Using the Interpreter CLI

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

* Pause after program execution (by requesting user input):  
  `--no-exit`  

* Launch interactive session in CAS mode (save and print out evalution result for each expression):  
  `--cas`

* Set max allocatable heap capacity (in bytes):  
  `--max-mem=123`  
  
* Set initial heap capacity (in bytes):  
  `--initial-mem=123`  
  
* Allocate the whole heap fully (capacity is determined by the `--max-mem` value) on interpreter launch:  
  `--allocate-fully`

* Disable the heap limit. Stops the interpreter from throwing an exception on allocating more than max heap capacity:  
  `--no-heap-limit`

* Print version string:  
  `--version`


## Building Methan0l

**Build requirements:**  

* CMake 3.21 or newer
* Any C++20 compiler
* vcpkg (with `VCPKG_ROOT` environment variable set)

Build release (artifacts under `build/runtime/release`):

```bash
cmake --preset release && cmake --build --preset release --parallel
```

Build debug (artifacts under `build/runtime/debug`):

```bash
cmake --preset debug && cmake --build --preset debug --parallel
```

Install the runtime and development headers:

```
cmake --install build --prefix /opt/methan0l
```

Create the release archive with:

```
cmake --build build --target package
```

## Tools

* [Methan0l IDE](https://github.com/Hitonoriol/Methan0l-IDE) - a mini IDE for Methan0l written in java.
