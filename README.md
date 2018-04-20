# bimap
Bimap (a map allowing access both by key and value). It therefore implies uniqueness of (key, value) pairs.
Two versions are defined, `unordered_bimap` and `bimap` (based on STL `unordered_map` and `map` respectively).
Bimap allows iteration of keys and of values. (`bimap` also sorts keys and values and allows reverse iteration).

# How to use
* `#include "bimap.h"` and have fun.
* `test.cpp` shows all available operations on both bimap versions.
