Header-only parsing and formatting of file sizes
==================================================

Small utility header for human-formatted input and output of file sizes in bytes:

* `5.55k`, `5.55 k`, `5.55kB`, `  5.55 kiB ` to number of bytes
* available case-sensitive prefixes are `k`, `M`, `G`, `T`, `P` (decimal) and `ki`, `Mi`, `Gi`, `Ti`, `Pi` (binary)
* throws exceptions for invalid inputs
* formats to 3-digit precision with affixed decimal units (e.g. `5.55 kB`)
* defined `operator<<` for iostream output
* defines c++ literals in `bytesize::literals` namespace

How to use
----------

* to parse a `std::string`, use `bytesize::bytesize::parse` static method
* to output, use `bytesize::bytesize::format`
* output to `std::ostream` via `operator<<`
* output to [fmt](https://github.com/fmtlib/fmt) with `{}` automatically — use `BYTESIZE_FMTLIB_FORMATTER` to declare the tempate (`fmt/fmt.h` must be included before that)
* `using namespace bytesize::literals` to do things like `5.55_kB` in your source

