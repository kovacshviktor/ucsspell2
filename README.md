[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL_v2.1-blue.svg)](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.en.html)
[![License: MPL 1.1](https://img.shields.io/badge/License-MPL_1.1-brightgreen.svg)](https://www.mozilla.org/en-US/MPL/1.1/)
[![Status](https://img.shields.io/badge/Status-Experimental-orange.svg)](#status)

**ucsspell2** is an experimental spell checking and morphological analysis engine built upon the upstream **Hunspell** codebase as of **August 30, 2026**. 

The primary objective of `ucsspell2` is to modernize and extend the core Hunspell engine to natively support full modern Unicode, seamless compile-time data synchronization with evolving Unicode standards, and automated graphical source code self-documentation.

---

## 🎯 Key Objectives & Features

### 1. Full Unicode BMP & SMP Range Support
Traditional spell checking engines often face limitations when processing characters beyond the Basic Multilingual Plane (BMP, `U+0000`–`U+FFFF`). `ucsspell2` extends morphological parsing, affix compression, n-gram suggestions, and compound word analysis to natively support the Supplementary Multilingual Plane (SMP, `U+010000`–`U+10FFFF`), including:
- Historical and ancient scripts (e.g., Old Turkic, Old Hungarian / Rovas, Linear B, Cuneiform).
- Modern constructed scripts and non-BMP writing systems (e.g., Deseret, Osage, Adlam, Tifinagh extensions).
- Emoji, symbols, and specialized linguistic/phonetic characters.

### 2. Up-to-Date Unicode Tracking with Compile-Time Updates
- **Automated UCD Integration:** Tooling to pull and parse the latest Unicode Character Database (UCD) properties, normalization forms, case folding, and character categories.
- **Compile-Time Table Generation:** Generates optimized character property tables and case-folding automata during compilation, ensuring the engine can be immediately adapted to new Unicode standard releases.

### 3. Graphical Source Code Self-Documentation (Doxygen)
- Automated graphical self-documentation generated exclusively from the C++ source code via **Doxygen** (with Graphviz `dot` backend).
- **Class Diagrams:** Visual class hierarchies, inheritance, and collaboration diagrams.
- **Call Diagrams:** Interactive direct call graphs and caller graphs (`CALL_GRAPH`, `CALLER_GRAPH`) for C++ functions and methods.

---

## 📜 Lineage & Relationship to Hunspell

`ucsspell2` is a continuation and experimental research branch based on the solid foundation of the Hunspell project (snapshot dated August 30, 2026). It preserves backward compatibility with standard Hunspell dictionary (`.dic`) and affix (`.aff`) formats while introducing extended syntax for wide-character mappings and Unicode properties.

---

## ⚖️ License

`ucsspell2` inherits the dual-licensing model of upstream Hunspell. All original Hunspell code and all subsequent enhancements in `ucsspell2` are dual-licensed under:

- **GNU Lesser General Public License, Version 2.1** (`LGPL-2.1-only` or `LGPL-2.1-or-later`)
- **Mozilla Public License, Version 1.1** (`MPL-1.1`)

You may use, distribute, and modify `ucsspell2` under the terms of either license.

---

## 🛠️ Building and Development


### Basic Build Instructions
```bash
git clone https://github.com/kovacshviktor/ucsspell2.git
cd ucsspell2

autoreconf -vfi  
./configure  
make  
sudo make install  
sudo ldconfig  
```
  
# Dependencies

Build only dependencies:

    g++ make autoconf automake autopoint libtool python 3.9+

Runtime dependencies:

|               | Mandatory        | Optional         |
|---------------|------------------|------------------|
|libhunspell    |                  |                  |
|hunspell tool  | libiconv gettext | ncurses readline |

# Compiling on GNU/Linux and Unixes

We first need to download the dependencies. On Linux, `gettext` and
`libiconv` are part of the standard library. On other Unixes we
need to manually install them.

For Ubuntu:

    sudo apt install autoconf automake autopoint libtool

Then run the following commands:

    autoreconf -vfi
    ./configure
    make
    sudo make install
    sudo ldconfig

# Generate PDF documentation
Additional dependencies
 - Doxyfile
 - Graphviz
 - pdflatex

autoreconf -vfi  
./configure --with-pdf-generation=yes  
make docs  

# Update and regenerate Unicode-depended codes
Additional dependencies
 - python 3.9+
 - curl or wget

autoreconf -vfi
./configure --with-python-codegen=yes --with-online-uc-data=yes
make
sudo make install
sudo ldconfig


For dictionary development, use the `--with-warnings` option of
configure.

For interactive user interface of Hunspell executable, use the
`--with-ui` option.

Optional developer packages:

  - ncurses (need for --with-ui), eg. libncursesw5 for UTF-8
  - readline (for fancy input line editing, configure parameter:
    --with-readline)

In Ubuntu, the packages are:

    libncurses5-dev libreadline-dev

# Compiling on OSX and macOS

On macOS for compiler always use `clang` and not `g++` because Homebrew
dependencies are build with that.

    brew install autoconf automake libtool gettext
    brew link gettext --force

Then run:

    autoreconf -vfi
    ./configure
    make
    sudo make install

# Compiling on Windows

## Compiling with Mingw64 and MSYS2

Download Msys2, update everything and install the following
    packages:

    pacman -S base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-libtool

Open Mingw-w64 Win64 prompt and compile the same way as on Linux, see
above.

## Compiling in Cygwin environment

Download and install Cygwin environment for Windows with the following
extra packages:

  - make
  - automake
  - autoconf
  - libtool
  - gcc-g++ development package
  - ncurses, readline (for user interface)
  - iconv (character conversion)
  - Python 3.9+ (for Unicode table generators)
  - Doxygen & Graphviz (for generating C++ class and call diagrams)

Then compile the same way as on Linux. Cygwin builds depend on
Cygwin1.dll.

# Debugging

It is recommended to install a debug build of the standard library:

    libstdc++6-6-dbg

For debugging we need to create a debug build and then we need to start
`gdb`.

    ./configure CXXFLAGS='-g -O0 -Wall -Wextra'
    make
    ./libtool --mode=execute gdb src/tools/hunspell

You can also pass the `CXXFLAGS` directly to `make` without calling
`./configure`, but we don't recommend this way during long development
sessions.
