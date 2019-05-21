// Copyright (c) The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or https://opensource.org/license/mit/.

use std::env;
use std::fs;
use std::io::ErrorKind;
use std::path::{Path, PathBuf};
use std::process::{Command, ExitCode, Stdio};

type LintError = String;
type LintResult = Result<(), LintError>;
type LintFn = fn() -> LintResult;

struct Linter {
    pub description: &'static str,
    pub name: &'static str,
    pub lint_fn: LintFn,
}

fn get_linter_list() -> Vec<&'static Linter> {
    vec![
        &Linter {
            description: "Check that all command line arguments are documented.",
            name: "doc",
            lint_fn: lint_doc
        },
        &Linter {
            description: "Check that no symbol from bitcoin-config.h is used without the header being included",
            name: "includes_build_config",
            lint_fn: lint_includes_build_config
        },
        &Linter {
            description: "Check that markdown links resolve",
            name: "markdown",
            lint_fn: lint_markdown
        },
        &Linter {
            description: "Check the default arguments in python",
            name: "py_mut_arg_default",
            lint_fn: lint_py_mut_arg_default,
        },
        &Linter {
            description: "Check that std::filesystem is not used directly",
            name: "std_filesystem",
            lint_fn: lint_std_filesystem
        },
        &Linter {
            description: "Check that subtrees are pure subtrees",
            name: "subtree",
            lint_fn: lint_subtree
        },
        &Linter {
