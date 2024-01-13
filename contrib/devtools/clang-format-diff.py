#!/usr/bin/env python3
#
# ===- clang-format-diff.py - ClangFormat Diff Reformatter ----*- python -*--===#
#
# Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ===------------------------------------------------------------------------===#

"""
This script reads input from a unified diff and reformats all the changed
lines. This is useful to reformat all the lines touched by a specific patch.
Example usage for git/svn users:

  git diff -U0 --no-color --relative HEAD^ | {clang_format_diff} -p1 -i
  svn diff --diff-cmd=diff -x-U0 | {clang_format_diff} -i

It should be noted that the filename contained in the diff is used unmodified
