# MIPSim: MIPS Simulator
A MIPS simulator implemented in C++. Originaly created for CS 3443 "Computer Systems" at Oklahoma State University for Dr. Cline.  The goal of this project was to create a command line MIPS simulator (without floating point operations and some system calls) capable of loading and running MIPS programs in a special format and facilitating debugging.

The structure of the input files is not binary or raw assembly but rather a special format based on the assembled output of the SPIM simulator. The reasoning for this format was that one could use SPIM to assemble a MIPS program and then copy and paste its output into a file for the simulator. Supporting this format was a requirment for the project; however, I may yet add functionality to load a MIPS program in Intel HEX format as well.

Unlike SPIM and other MIPS simulators, MIPSim simulates system calls as though they were implemented in hardware. This allows the simulator to run without providing it a kernel. Note that only some system calls are implemented and that handling the additional calls in software is not feasible.

## Doxygen
This project has been commented with Doxygen compatible comments. The resulting HTML files from Doxygen have been omitted from GitHub as they are not strictly part of the source code and are rather bulky; however, running Doxygen (optimized for C++) on this project's source code will produce a complete documentation in HTML. For more on Doxygen please visit their <a href="http://www.stack.nl/~dimitri/doxygen/" target="_blank">website</a>.

## UML Diagram
The <a href="https://creately.com/diagram/ik1cgqhg1/dpnmwwhRgcTcs4jko7Ui2uL7DCU%3D" target="_blank">UML diagram</a> for this project is hosted at Creately. Note that the diagram is not generated from code and thus changes to the code will not be reflected in the UML or vice versa.

## Compiling
This project has been tested to compile on Windows 7 with Visual Studio 2012 and Linux with gcc version 4.9.2 20150212 (Red Hat 4.9.2-6). Instructions for compiling on both OSs are given below for those who may not know how. If you compile this project on another OS/compiler please let me know which one and the settings so that I may add it to this list.

© Copyright 2016 Charles Duncan (CharlesETD@gmail.com)
