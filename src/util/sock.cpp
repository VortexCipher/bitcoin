// Copyright (c) 2020-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <common/system.h>
#include <compat/compat.h>
#include <logging.h>
#include <tinyformat.h>
#include <util/sock.h>
#include <util/syserror.h>
#include <util/threadinterrupt.h>
#include <util/time.h>

#include <memory>
#include <stdexcept>
#include <string>

#ifdef USE_POLL
#include <poll.h>
#endif

static inline bool IOErrorIsPermanent(int err)
{
    return err != WSAEAGAIN && err != WSAEINTR && err != WSAEWOULDBLOCK && err != WSAEINPROGRESS;
}

Sock::Sock(SOCKET s) : m_socket(s) {}

Sock::Sock(Sock&& other)
{
    m_socket = other.m_socket;
    other.m_socket = INVALID_SOCKET;
}

Sock::~Sock() { Close(); }

Sock& Sock::operator=(Sock&& other)
{
    Close();
    m_socket = other.m_socket;
    other.m_socket = INVALID_SOCKET;
    return *this;
}
