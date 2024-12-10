# Welcome To FOORT!

The code files are all in the `FOORT/src` subfolder.

## QUICK INSTALL GUIDE

### LINUX / MAC:

#### CMake

The executable can be built with CMake, which should be installed first (for regular Makefiles, see further). The advantage of this method it that, in principle, CMake
will automatically detect your relevant compilers.

One can then run

```
cmake -S . -B build
cmake --build build
```

which will cause the executable to be stored in the `bin` subdirectory. CMake can be cleaned by simply removing the relevant folders

```
rm -rf build
rm -rf bin
```

#### MacOS Troubleshooting

Recent versions of MacOS may encounter issues with CMake. The following seems to resolve current issues.
(Found in [this issue](https://gist.github.com/scivision/d69faebbc56da9714798087b56de925a))

```
export CXX=/opt/homebrew/bin/g++-14
export SDKROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX14.sdk/
```

For convenience, this is stored in `alternative_builds/MacOS_workaround.sh`, which should be executed with `source \the\path\` in the terminal. You may need to `rm -rf build` in order to start clean.

#### Old MakeFiles

Alternatively, old Makefiles can be found in `alternative_builds/old_makefiles`. These can be system dependant, however, but should be adaptable to your needs.

### WINDOWS VISUAL STUDIO

Use the `alternative_builds/Foort.sln` Visual Studio solution file and compile the FOORT project therein.

## OUTPUT

See in the [output](FOORT/Output) file for sample output files and Mathematica notebooks that process (and plot) this output. (The documentation will also have more information in the future.)

## Documentation

See the [Documentation](./docs/Documentation.pdf) for more information on configuration options, how to add your own Metric, Diagnostic, or Termination, and how to process output.

You can consult the `doxygen` created documentation to find out more about the structure of the code.
If you have downloaded the repository, you can achieve this by opening `docs/html/index.html` in a browser.
A PDF version of this documentation is also available in `docs/latex`. The automated documentation is still under construction, however.
