# Methan0l Modules

### Build individual modules

```
make module-name
```  

Where `name` is the name of the folder which contains the module's sources.  

### Use modules inside Methan0l

You can load `.so` modules as **Box Units** in Methan0l using the following syntax:  

```
module = load_module(path)
```

Where `.so` extension can be omitted from the `path`.  

Everything defined inside the `module` can then be used by accessing its scope: `module.foo(...)` or by importing its contents into the current scope: `module.import()`.