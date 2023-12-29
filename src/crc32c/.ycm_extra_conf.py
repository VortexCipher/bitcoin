# Copyright 2017 The CRC32C Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
"""YouCompleteMe configuration that interprets a .clang_complete file.

This module implementes the YouCompleteMe configuration API documented at:
https://github.com/ycm-core/ycmd#ycm_extra_confpy-specification

The implementation loads and processes a .clang_complete file, documented at:
https://github.com/xavierd/clang_complete/blob/master/README.md
"""

import os

