# Newlib for Circle

This is a fork of [newlib](https://sourceware.org/newlib/) for using it with [Circle](https://github.com/rsta2/circle).

## Project Home

The project is hosted in GitHub:

https://github.com/smuehlst/circle-newlib

## Getting Started

This repository can only be used in the context of the
[circle-stdlib](https://github.com/smuehlst/circle-stdlib) project.
[circle-stdlib](https://github.com/smuehlst/circle-stdlib) contains
documentation and scripts for building newlib in combination with Circle.

## Modifying Newlib for Circle

The modifications for circle-stdlib are mainly in the subdirectory libgloss/circle.

When files are added, die Newlib build system must be updated. For this autoconf2.69
must be installed.

To update the build system:

```
$ cd libgloss
$ autoreconf2.69
```
