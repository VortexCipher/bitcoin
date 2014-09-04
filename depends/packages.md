Each recipe consists of 3 main parts: defining identifiers, setting build
variables, and defining build commands.

The package "mylib" will be used here as an example

General tips:
- mylib_foo is written as $(package)_foo in order to make recipes more similar.
- Secondary dependency packages relative to the bitcoin binaries/libraries (i.e.
  those not in `ALLOWED_LIBRARIES` in `contrib/devtools/symbol-check.py`) don't
  need to be shared and should be built statically whenever possible. See
  [below](#secondary-dependencies) for more details.

## Identifiers
Each package is required to define at least these variables:

    $(package)_version:
    Version of the upstream library or program. If there is no version, a
    placeholder such as 1.0 can be used.

    $(package)_download_path:
