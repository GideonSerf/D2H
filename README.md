# D2H
This is a simple tool to convert image, font and sound files into arrays for use with raylib.

## Usage
```
d2h <Input file> <Output file> [Extension]
```

This will produce a file containing an array with the same name as the output file(therefore no spaces in output file names).

## Example

```
d2h in.png out.h
```

This will produce the following in out.h:
```
const char* out_ext = ".png";

int out_size = N;

unsigned char out[] = {...};
```

Where N is the amount of bytes stored in the header.

---

To use this in your raylib project you would then do the following:

```
#include "out.h"

Image outImg = LoadImageFromMemory(out_ext, out, out_size);
```
