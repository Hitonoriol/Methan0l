# Methan0l Modules

### Build individual modules

```
cd modules && make module-name
```  

Where `name` is the name of the folder which contains the module's sources.  

### Use modules inside Methan0l

You can load `.so` and `.mt0` modules as **Box Units** in Methan0l using the following syntax:  

```
module = load(path)
```

Where `.so` / `.mt0` extension or even the filename of the module binary / script itself can be omitted from the `path`.  

Everything defined inside the `module` can then be used by accessing its scope: `module.foo(...)` or by importing its contents into the current scope: `module.import()`.  
\
Modules can also be imported in single expression:  
```
using_module: path
```  
This effectively loads a module from `path` and imports its contents into the current scope.