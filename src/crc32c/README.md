# CRC32C

[![Build Status](https://travis-ci.org/google/crc32c.svg?branch=master)](https://travis-ci.org/google/crc32c)
[![Build Status](https://ci.appveyor.com/api/projects/status/moiq7331pett4xuj/branch/master?svg=true)](https://ci.appveyor.com/project/pwnall/crc32c)

New file format authors should consider
[HighwayHash](https://github.com/google/highwayhash). The initial version of
this code was extracted from [LevelDB](https://github.com/google/leveldb), which
is a stable key-value store that is widely used at Google.

This project collects a few CRC32C implementations under an umbrella that
dispatches to a suitable implementation based on the host computer's hardware
