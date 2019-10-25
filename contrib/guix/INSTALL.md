# Guix Installation and Setup

This only needs to be done once per machine. If you have already completed the
installation and setup, please proceed to [perform a build](./README.md).

Otherwise, you may choose from one of the following options to install Guix:

1. Using the official **shell installer script** [⤓ skip to section][install-script]
   - Maintained by Guix developers
   - Easiest (automatically performs *most* setup)
   - Works on nearly all Linux distributions
   - Only installs latest release
   - Binary installation only, requires high level of trust
   - Note: The script needs to be run as root, so it should be inspected before it's run
2. Using the official **binary tarball** [⤓ skip to section][install-bin-tarball]
   - Maintained by Guix developers
   - Normal difficulty (full manual setup required)
   - Works on nearly all Linux distributions
   - Installs any release
   - Binary installation only, requires high level of trust
3. Using fanquake's **Docker image** [↗︎ external instructions][install-fanquake-docker]
   - Maintained by fanquake
   - Easy (automatically performs *some* setup)
   - Works wherever Docker images work
   - Installs any release
   - Binary installation only, requires high level of trust
4. Using a **distribution-maintained package** [⤓ skip to section][install-distro-pkg]
   - Maintained by distribution's Guix package maintainer
   - Normal difficulty (manual setup required)
   - Works only on distributions with Guix packaged, see: https://repology.org/project/guix/versions
   - Installs a release decided on by package maintainer
   - Source or binary installation depending on the distribution
5. Building **from source** [⤓ skip to section][install-source]
   - Maintained by you
   - Hard, but rewarding
   - Can be made to work on most Linux distributions
   - Installs any commit (more granular)
   - Source installation, requires lower level of trust

## Options 1 and 2: Using the official shell installer script or binary tarball

The installation instructions for both the official shell installer script and
the binary tarballs can be found in the GNU Guix Manual's [Binary Installation
section](https://guix.gnu.org/manual/en/html_node/Binary-Installation.html).

Note that running through the binary tarball installation steps is largely
equivalent to manually performing what the shell installer script does.

Note that at the time of writing (July 5th, 2021), the shell installer script
automatically creates an `/etc/profile.d` entry which the binary tarball
installation instructions do not ask you to create. However, you will likely
need this entry for better desktop integration. Please see [this
section](#add-an-etcprofiled-entry) for instructions on how to add a
`/etc/profile.d/guix.sh` entry.

Regardless of which installation option you chose, the changes to
`/etc/profile.d` will not take effect until the next shell or desktop session,
so you should log out and log back in.

## Option 3: Using fanquake's Docker image

Please refer to fanquake's instructions
[here](https://github.com/fanquake/core-review/tree/master/guix).

## Option 4: Using a distribution-maintained package

Note that this section is based on the distro packaging situation at the time of
writing (July 2021). Guix is expected to be more widely packaged over time. For
an up-to-date view on Guix's package status/version across distros, please see:
https://repology.org/project/guix/versions

### Debian / Ubuntu

Guix is available as a distribution package in [Debian
](https://packages.debian.org/search?keywords=guix) and [Ubuntu
](https://packages.ubuntu.com/search?keywords=guix).

To install:
```sh
sudo apt install guix
```
