# Copyright (c) 2013-2020 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# Pattern rule to print variables, e.g. make print-top_srcdir
print-%: FORCE
	@echo '$*'='$($*)'

ACLOCAL_AMFLAGS = -I build-aux/m4
SUBDIRS = src
if ENABLE_MAN
SUBDIRS += doc/man
endif
.PHONY: deploy FORCE
.INTERMEDIATE: $(COVERAGE_INFO)

BITCOIND_BIN=$(top_builddir)/src/$(BITCOIN_DAEMON_NAME)$(EXEEXT)
BITCOIN_QT_BIN=$(top_builddir)/src/qt/$(BITCOIN_GUI_NAME)$(EXEEXT)
BITCOIN_TEST_BIN=$(top_builddir)/src/test/$(BITCOIN_TEST_NAME)$(EXEEXT)
BITCOIN_CLI_BIN=$(top_builddir)/src/$(BITCOIN_CLI_NAME)$(EXEEXT)
BITCOIN_TX_BIN=$(top_builddir)/src/$(BITCOIN_TX_NAME)$(EXEEXT)
BITCOIN_UTIL_BIN=$(top_builddir)/src/$(BITCOIN_UTIL_NAME)$(EXEEXT)
BITCOIN_WALLET_BIN=$(top_builddir)/src/$(BITCOIN_WALLET_TOOL_NAME)$(EXEEXT)
BITCOIN_NODE_BIN=$(top_builddir)/src/$(BITCOIN_MP_NODE_NAME)$(EXEEXT)
BITCOIN_GUI_BIN=$(top_builddir)/src/$(BITCOIN_MP_GUI_NAME)$(EXEEXT)
BITCOIN_WIN_INSTALLER=$(PACKAGE)-$(PACKAGE_VERSION)-win64-setup$(EXEEXT)

empty :=
space := $(empty) $(empty)

OSX_APP=Bitcoin-Qt.app
OSX_VOLNAME = $(subst $(space),-,$(PACKAGE_NAME))
OSX_ZIP = $(OSX_VOLNAME).zip
OSX_DEPLOY_SCRIPT=$(top_srcdir)/contrib/macdeploy/macdeployqtplus
