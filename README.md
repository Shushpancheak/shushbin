# shushbin
Binary translator from custom bytecode (shushexe) to Windows executable (PE format).

## Building
Windows:
In Visual Studio 2017+ open folder with repository as a CMake Project.
Add `-UBUILD_TESTS -DBUILD_TESTS=ON` to CMake args for building tests.

## How to use
Run the built executable with the first arg set to file name of a file containing shush bytecode you want to translate.
The program will produce a Windows executable named after the file processed. 

## Build documentation
```shell
doxygen
```

Documentation will be generated in the directory named `docs`.
