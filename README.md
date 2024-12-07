# Welcome To FOORT!

The code files are all in the `FOORT` subfolder.

See the [Documentation](./Documentation.pdf) for more information on configuration options, how to add your own Metric, Diagnostic, or Termination, and how to process output.

## QUICK INSTALL GUIDE

### WINDOWS VISUAL STUDIO

Use the Foort.sln Visual Studio solution file and compile the FOORT project therein.

### LINUX / MAC:

The executable can be built with `cmake`, which should be installed first. One can then run

```
cmake -S . -B build
cmake --build build
```

which will cause the executable to be stored in the `bin` subdirectory.

#### MacOS Troubleshooting

```
export CXX=/opt/homebrew/opt/llvm/bin/clang++
export CPPFLAGS="-I/usr/local/opt/llvm/include"
```

Alternatively, old Makefiles can be found in XXXX

OLD BELOW

All code files are in the `FOORT` subfolder. Run `make` in this folder to compile. You will need to have installed the libconfig library beforehand (see the [Documentation](./Documentation.pdf)).

Alternatively, rename `makefile_precompiledmode` to `makefile` (and rename the original makefile). Then also comment out the line

```
#define CONFIGURATION_MODE
```

at the top of `Config.h` Then running `make` should be possible without installing the libconfig library.

## OUTPUT

See in the [output](FOORT/Output) file for sample output files and Mathematica notebooks that process (and plot) this output. (The documentation will also have more information in the future.)
