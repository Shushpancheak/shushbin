# shushbin
Binary translator from custom bytecode (shushexe) to Windows executable (PE format).

## Explaination
Shushexe is meant to be run on a custom soft-processor that has four QWORD sized registers for double-precision values, as well as a stack which is able to hold said numbers. The arithmetic operations are performed at the top of a stack. The jump commands are also present. There is also `in` and `out` commands that print and scan respectively values that are/will be put on the stack.

## Example
In the `resources` folder you can find a factorial.shushexe file, which is meant to find a factorial of a given number. You can translate this file into PE32 using shushbin, which will hopefully emit a file named factorial.exe, which is a program that will take an input of double-precision value, find its factorial and print it out. 

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
