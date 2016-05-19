// Based on the https://github.com/arun11299/cpp-subprocess project.

/*!

Documentation for C++ subprocessing library.

@copyright The code is licensed under the [MIT
  License](http://opensource.org/licenses/MIT):
  <br>
  Copyright &copy; 2016-2018 Arun Muralidharan.
  <br>
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  <br>
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  <br>
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

@author [Arun Muralidharan]
@see https://github.com/arun11299/cpp-subprocess to download the source code

@version 1.0.0
*/

#ifndef BITCOIN_UTIL_SUBPROCESS_H
#define BITCOIN_UTIL_SUBPROCESS_H

#include <util/syserror.h>

#include <algorithm>
#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <future>
#include <initializer_list>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#if (defined _MSC_VER) || (defined __MINGW32__)
  #define __USING_WINDOWS__
#endif

#ifdef __USING_WINDOWS__
  #include <codecvt>
#endif

extern "C" {
#ifdef __USING_WINDOWS__
  #include <Windows.h>
  #include <io.h>
  #include <cwchar>

  #define close _close
  #define open _open
  #define fileno _fileno
#else
  #include <sys/wait.h>
  #include <unistd.h>
#endif
  #include <csignal>
  #include <fcntl.h>
  #include <sys/types.h>
}

/*!
 * Getting started with reading this source code.
 * The source is mainly divided into four parts:
 * 1. Exception Classes:
 *    These are very basic exception classes derived from
 *    runtime_error exception.
 *    There are two types of exception thrown from subprocess
 *    library: OSError and CalledProcessError
 *
 * 2. Popen Class
 *    This is the main class the users will deal with. It
 *    provides with all the API's to deal with processes.
 *
 * 3. Util namespace
 *    It includes some helper functions to split/join a string,
 *    reading from file descriptors, waiting on a process, fcntl
 *    options on file descriptors etc.
 *
 * 4. Detail namespace
 *    This includes some metaprogramming and helper classes.
 */


namespace subprocess {

// Max buffer size allocated on stack for read error
// from pipe
static const size_t SP_MAX_ERR_BUF_SIZ = 1024;

// Default buffer capacity for OutBuffer and ErrBuffer.
// If the data exceeds this capacity, the buffer size is grown
// by 1.5 times its previous capacity
static const size_t DEFAULT_BUF_CAP_BYTES = 8192;


/*-----------------------------------------------
 *    EXCEPTION CLASSES
 *-----------------------------------------------
 */

/*!
 * class: CalledProcessError
 * Thrown when there was error executing the command.
 * Check Popen class API's to know when this exception
 * can be thrown.
 *
 */
class CalledProcessError: public std::runtime_error
{
public:
  int retcode;
  CalledProcessError(const std::string& error_msg, int retcode):
    std::runtime_error(error_msg), retcode(retcode)
  {}
};


/*!
 * class: OSError
 * Thrown when some system call fails to execute or give result.
 * The exception message contains the name of the failed system call
 * with the stringisized errno code.
 * Check Popen class API's to know when this exception would be
