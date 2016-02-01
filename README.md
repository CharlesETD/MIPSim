# MIPSim: MIPS Simulator
A MIPS simulator implemented in C++. Originaly created for CS 3443 "Computer Systems" at Oklahoma State University for Dr. Cline.  The goal of this project was to create a command line MIPS simulator (without floating point operations and some system calls) capable of loading and running MIPS programs in a special format and facilitating debugging.

The structure of the input files is not binary or raw assembly but rather a special format based on the assembled output of the SPIM simulator. The reasoning for this format was that one could use SPIM to assemble a MIPS program and then copy and paste its output into a file for the simulator. Supporting this format was a requirement for the project; however, I may yet add functionality to load a MIPS program in Intel HEX format as well.

Unlike SPIM and other MIPS simulators, MIPSim simulates system calls as though they were implemented in hardware. This allows the simulator to run without providing it a kernel. Note that only some system calls are implemented and that handling the additional calls in software is not feasible.

## Doxygen
This project has been commented with Doxygen compatible comments. The resulting HTML files from Doxygen have been omitted from GitHub as they are not strictly part of the source code and are rather bulky; however, running Doxygen (optimized for C++) on this project's source code will produce a complete documentation in HTML. For more on Doxygen please visit their <a href="http://www.stack.nl/~dimitri/doxygen/" target="_blank">website</a>.

## UML Diagram
The <a href="https://creately.com/diagram/ik1cgqhg1/dpnmwwhRgcTcs4jko7Ui2uL7DCU%3D" target="_blank">UML diagram</a> for this project is hosted at Creately. Note that the diagram is not generated from code and thus changes to the code will not be reflected in the UML or vice versa.

## Compiling
This project has been tested to compile on Windows 7 with Visual Studio 2012 and Linux with gcc version 4.9.2 20150212 (Red Hat 4.9.2-6). Instructions for compiling on both OSs are given below for those who may not know how. If you compile this project on another OS/compiler please let me know which one and the settings so that I may add it to this list.

### Windows | Visual Studio 2012
1. Create a new empty project named "MIPSim".
2. Clone the contents of "MIPSim" from here into the newly made "MIPSim/MIPSim" folder in your Visual Studio Project directory.
3. In Visual Studio right click the "Source Files" directory in the Solution Explorer and select Add->Existing Item.
4. Add all of the .cpp files.
5. Add all of the .h files to the "Header Files" directory in the Solution Explorer following the same procedure as above.
6. Right click "MIPSim" in the Solution Explorer and select "Properties".
7. Select the "Linker" tab and then the "System" tab.
8. Change "SubSystem" to "Console" and click "Apply" but do not click "Ok" yet.
9. At the top of the window change "Configuration" to "Release" and repeat step 8. You may now click "Ok".
10. Compile the program by pressing F7 and run by pressing F5. Note that by default Visual Studio will compile in Debug mode rather than release. A dropdown menu on the menu bar will allow you to switch between either Release or Debug mode.

### Linux | G++ 4.9.2
1. Clone the "MIPSim" directory to your machine.
2. Make two more directories titled "Release" and "Debug" in the same directory you cloned the "MIPSim" directory into.
3. At this point you should have 3 folders "Release", "Debug", and "MIPSim" all inside of your current directory.
4. To compile Debug mode type: g++ -std=c++11 -Og MIPSim/*.cpp -o Debug/MIPSim
5. To compile Release mode type: g++ -std=c++11 -O3 MIPSim/*.cpp -o Release/MIPSim

As a final note, building Debug mode in Visual Studio will cause automated tests to be built in as well. These automated tests run only during startup of the program in Debug mode. To include these tests in other compilers you must manually define "_DEBUG" for your Debug builds. These tests are not inclusive and are meant only for basic testing.

## Usage
MIPSim accepts only command line arguments. The following is every possible argument with proper syntax and examples:

### Load and Run a ROM
Loads the specified ROM file and begins running it immediately.

| Argument | Syntax    | Example   |
|----------|-----------|-----------|
| `N/A`     | `MIPSim <rom filename>` | `MIPSim "Examples\Rot13.txt"` |

### Load and Debug a ROM
Loads the specified ROM but initiates a manual step-through debugger. Once the debugger is running you will see the current value of the program counter and instruction displayed for each step. You may simply press enter to advance the program counter. You may also type a memory address in hex (Ex. 0x00AB1000) to view the byte at that location as well as the full word which that location is a part of. You can also view the contents of a register by typing its ID after a dollar sign (Ex. $13).

| Argument | Syntax                                     | Example                                    |
|----------|--------------------------------------------|--------------------------------------------|
| `-d`       | `MIPSim <rom filename> -d` | `MIPSim "Examples\Rot13.txt" -d` |

## Examples
This folder contains example ROMS to help you understand the format accepted by MIPSim. Teaching MIPS assembly is beyond the scope of this project; however, the files do contain tutorials of the ROM format.

### Rot13
A simple rot13 cipher program. The user is prompted for a string up to 128 characters long and is given the resulting ciphertext. This ROM also contains short tutorials on the formatting of ROMS in its comments.

© Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
