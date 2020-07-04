ansak-string
============

Copyright 2015, Arthur N. Klassen; see LICENSE for usage guidelines (BSD 2-clause)

This library started as the first piece of a larger project that has been stalled. It's a simple string library. With the release of version 2.0, I am returning to work on the larger project now, and the results will be available under the **ansak-tools** branch of this repository. String changes will be merged into the master branch and pushed down to **ansak-lib** (*this strategy is changing, see below*).

**ansak-string** is mostly about purifying input and performing conversions more simply than other available APIs. If your string (whether it's 7-bit ASCII, UTF-8, UTF-16, UCS-2 or UTF-32/UCS-4) makes it through my APIs (into 7-bit ASCII, UTF-8 etc.), then that's what it is. No more, no less. This includes filtering for publicly assigned Unicode values, currently following Unicode 13, supporting through cmake switch every major version back to 7.0.

A simple pair of split and join templates, based on character types and using basic\_string for their processing are also present.

Commercial Support:
-------------------

Available on request, subject to reasonable terms we can agree on. Please contact me via anklassen (at) onepost dot net.

Requests for more conversions from Windows Code Pages and/or ISO/ECMA 8859 encoding standards will be considered individually.

Additional stuff:
-----------------

* unicodeLength gives number of UCS-4 codes (regardless of composite/composable points) in a string
* split and join, like python's string and array methods
* all-Unicode-sensitive "tolower". It can operate in "Turkic" mode if you pass in an (optional) constant C-string for a Turkic language's ISO-639 two or three character code. In this mode, I-dot and dotless-i are handled correctly for those languages.

ISO-639 codes for Turkic languages:

* Azerbaizhani: "az", "azb", "aze", "azj"
* Kazakh: "kk", "kaz"
* Tatar: "tt", "tat"
* Turkish "tr", "tur"

Long-term Goal:
---------------

The ultimate goal of these libraries is to support a simple household expense tracking program that I plan to release under an appropriate open source license, probably using Qt for User Interface, sqlite3 for storage and implemented with the best that C++11/14/7 can supply.

For the moment, in one change to facilitate that goal, I am adding `FindANSAK.cmake` to **ansak-string**, to be mirrored and kept up to date in any upcoming libraries, and will be taking **ansak-lib** out to be its own repo, dependent on **ansak-string**, using `find_package` to locate and incorporate that dependency, regardless of Unicode version being used.
