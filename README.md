<img alt="Falcon" align="left" width="256px" height="256px" src="./logos/falcon.svg">

# Falcon Scripting Engine

 A scripting engine written in C++17.

## Features

* Register-based Virtual Machine with custom bytecode.
* Custom Reflection using Falcon Abstract Language Interface(FALI).
* Custom Assembly language with Assembler.
* Debugging support.

### Planned for the future
* Compiler Frontend for FalconScript (Custom Language).
* Unified Optimization and Assembly Generation using FALI AST and IR.

### Building
#### Configuration
On Windows:  
* Run ```configure.bat``` to generate project files for Visual Studio 2019. 

On Linux:  
* Run ```configure.sh``` to generate makefiles.

#### Compiling
##### Configurations:
* Debug: Compile with debug symbols, the macro DEBUG and no optimizations.
* Release: Compile fully optimized without debug symbols and the DEBUG macro.

##### Platforms:
* Windows32: Compile for 32-bit windows.
* Windows64: Compile for 64-bit windows.
* Linux32: Compile for 32-bit linux.
* Linux64: Compile for 64-bit linux.

On Windows:
* Build the solution.

On Linux:
* run ```make config=<lowercase_configuration_name>_<lowercase_platform_name> all```

The libraries will be inside ```lib/<configuration>/<platform>/``` and the binaries will be inside
```bin/<configuration>/<platform>/```.

### Usage
Refer to the ```--help``` option of the binaries for details on their usage.

The API is currently undocumented.

### Running the examples
After compiling, run these commands from root directory to compile the example assemblies:

On Windows:  
```
\path\to\fasm examples\factorial.fasm -o examples\factorial.fali
\path\to\fasm examples\helloworld.fasm -o examples\helloworld.fali
```

On Linux:  
```
/path/to/fasm examples/factorial.fasm -o examples/factorial.fali
/path/to/fasm examples/helloworld.fasm -o examples/helloworld.fali
```

Replace `/path/to/fasm` with the path to `fasm`. Refer to [Building](https://github.com/SarojKumar10/FalconScriptingEngine#Building)
for more info on where binaries are located.

Run them as follows:

On Windows:  
```
\path\to\falcon examples\<ExampleName>.fali -e <ArgsForExample>
```

On Linux:
```
/path/to/falcon examples/<ExampleName>.fali -e <ArgsForExample>
```

Refer to the comment at the beginning of the corresponding example's `.fasm` file for information on what args should be passed.
