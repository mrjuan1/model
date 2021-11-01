# Model

A small tool for extracting vertex data from Blender mesh objects and indexing that data for use with E01.

## Contents

- [Requirements](#requirements)
- [Configuration](#configuration)
- [Building](#building)
- [Usage](#usage)
- [Development](#development)

[Back to top](#model)

## Requirements

For building, a C compiler and compatible C library of your choice can be used. This project was developed using GCC and GLIBC.

For extracting vertex data from Blender mesh objects, Blender itself will be required. Blender 2.93.5 was used while developing this project.

To run or debug this tool, a valid Blender project file will be required too. More details are specified in the [Usage](#usage) section below.

[Back to top](#model)

## Configuration

This project can be configured for building using the `./configure` script. This script allows you to specify which platform you'd like to build for.

Currently, only `linux` is supported with plans to add support for Windows later.

[Back to top](#model)

## Building

After [configuring](#configuration), the project can be built using the `make` command. This will build the executable in debug mode and attempt to generate an indexed vertex file from the specified `.blend` file (see [Usage](#usage) below).

For more `make` options, run `make help`.

[Back to top](#model)

## Usage

For executable usage, you can run `./model --help`.

This project file needs at least one triangulated and UV-mapped mesh object with a single UV map. The object, its mesh and its UV map all need to have the same name. Once this is prepared, either place the `.blend` file in this directory or in this directory's parent directory. Next, update `export.py` and `common.mk` to make use of the new model and its name.

[Back to top](#model)

## Development

To debug this project, GDB will be required. It can be launched using `make debug`. Support for debugging via VS Code's C/C++ extension is also available.

[Back to top](#model)
