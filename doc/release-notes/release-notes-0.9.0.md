Bitcoin Core version 0.9.0 is now available from:

  https://bitcoin.org/bin/0.9.0/

This is a new major version release, bringing both new features and
bug fixes.

Please report bugs using the issue tracker at github:

  https://github.com/bitcoin/bitcoin/issues

How to Upgrade
--------------

If you are running an older version, shut it down. Wait until it has completely
shut down (which might take a few minutes for older versions), uninstall all
earlier versions of Bitcoin, then run the installer (on Windows) or just copy
over /Applications/Bitcoin-Qt (on Mac) or bitcoind/bitcoin-qt (on Linux).

If you are upgrading from version 0.7.2 or earlier, the first time you run
0.9.0 your blockchain files will be re-indexed, which will take anywhere from 
30 minutes to several hours, depending on the speed of your machine.

On Windows, do not forget to uninstall all earlier versions of the Bitcoin
client first, especially if you are switching to the 64-bit version.

Windows 64-bit installer
-------------------------

New in 0.9.0 is the Windows 64-bit version of the client. There have been
frequent reports of users running out of virtual memory on 32-bit systems
during the initial sync. Because of this it is recommended to install the
64-bit version if your system supports it.

NOTE: Release candidate 2 Windows binaries are not code-signed; use PGP
and the SHA256SUMS.asc file to make sure your binaries are correct.
In the final 0.9.0 release, Windows setup.exe binaries will be code-signed.

OSX 10.5 / 32-bit no longer supported
-------------------------------------

