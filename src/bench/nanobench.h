//  __   _ _______ __   _  _____  ______  _______ __   _ _______ _     _
//  | \  | |_____| | \  | |     | |_____] |______ | \  | |       |_____|
//  |  \_| |     | |  \_| |_____| |_____] |______ |  \_| |_____  |     |
//
// Microbenchmark framework for C++11/14/17/20
// https://github.com/martinus/nanobench
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019-2023 Martin Leitner-Ankerl <martin.ankerl@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef ANKERL_NANOBENCH_H_INCLUDED
#define ANKERL_NANOBENCH_H_INCLUDED

// see https://semver.org/
#define ANKERL_NANOBENCH_VERSION_MAJOR 4  // incompatible API changes
#define ANKERL_NANOBENCH_VERSION_MINOR 3  // backwards-compatible changes
#define ANKERL_NANOBENCH_VERSION_PATCH 11 // backwards-compatible bug fixes

///////////////////////////////////////////////////////////////////////////////////////////////////
// public facing api - as minimal as possible
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <chrono>        // high_resolution_clock
#include <cstring>       // memcpy
#include <iosfwd>        // for std::ostream* custom output target in Config
#include <string>        // all names
#include <unordered_map> // holds context information of results
#include <vector>        // holds all results

#define ANKERL_NANOBENCH(x) ANKERL_NANOBENCH_PRIVATE_##x()

#define ANKERL_NANOBENCH_PRIVATE_CXX() __cplusplus
#define ANKERL_NANOBENCH_PRIVATE_CXX98() 199711L
#define ANKERL_NANOBENCH_PRIVATE_CXX11() 201103L
#define ANKERL_NANOBENCH_PRIVATE_CXX14() 201402L
#define ANKERL_NANOBENCH_PRIVATE_CXX17() 201703L

#if ANKERL_NANOBENCH(CXX) >= ANKERL_NANOBENCH(CXX17)
#    define ANKERL_NANOBENCH_PRIVATE_NODISCARD() [[nodiscard]]
#else
#    define ANKERL_NANOBENCH_PRIVATE_NODISCARD()
#endif

#if defined(__clang__)
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_PADDED_PUSH() \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wpadded\"")
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_PADDED_POP() _Pragma("clang diagnostic pop")
#else
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_PADDED_PUSH()
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_PADDED_POP()
#endif

#if defined(__GNUC__)
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_EFFCPP_PUSH() _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Weffc++\"")
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_EFFCPP_POP() _Pragma("GCC diagnostic pop")
#else
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_EFFCPP_PUSH()
#    define ANKERL_NANOBENCH_PRIVATE_IGNORE_EFFCPP_POP()
#endif

#if defined(ANKERL_NANOBENCH_LOG_ENABLED)
#    include <iostream>
#    define ANKERL_NANOBENCH_LOG(x)                                                 \
        do {                                                                        \
            std::cout << __FUNCTION__ << "@" << __LINE__ << ": " << x << std::endl; \
        } while (0)
#else
#    define ANKERL_NANOBENCH_LOG(x) \
        do {                        \
        } while (0)
#endif

#define ANKERL_NANOBENCH_PRIVATE_PERF_COUNTERS() 0
#if defined(__linux__) && !defined(ANKERL_NANOBENCH_DISABLE_PERF_COUNTERS)
#    include <linux/version.h>
#    if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0)
// PERF_COUNT_HW_REF_CPU_CYCLES only available since kernel 3.3
// PERF_FLAG_FD_CLOEXEC since kernel 3.14
#        undef ANKERL_NANOBENCH_PRIVATE_PERF_COUNTERS
#        define ANKERL_NANOBENCH_PRIVATE_PERF_COUNTERS() 1
#    endif
#endif

#if defined(__clang__)
#    define ANKERL_NANOBENCH_NO_SANITIZE(...) __attribute__((no_sanitize(__VA_ARGS__)))
#else
#    define ANKERL_NANOBENCH_NO_SANITIZE(...)
#endif

#if defined(_MSC_VER)
#    define ANKERL_NANOBENCH_PRIVATE_NOINLINE() __declspec(noinline)
#else
#    define ANKERL_NANOBENCH_PRIVATE_NOINLINE() __attribute__((noinline))
#endif

// workaround missing "is_trivially_copyable" in g++ < 5.0
// See https://stackoverflow.com/a/31798726/48181
#if defined(__GNUC__) && __GNUC__ < 5
#    define ANKERL_NANOBENCH_IS_TRIVIALLY_COPYABLE(...) __has_trivial_copy(__VA_ARGS__)
#else
#    define ANKERL_NANOBENCH_IS_TRIVIALLY_COPYABLE(...) std::is_trivially_copyable<__VA_ARGS__>::value
#endif

// noexcept may be missing for std::string.
// See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=58265
#define ANKERL_NANOBENCH_PRIVATE_NOEXCEPT_STRING_MOVE() std::is_nothrow_move_assignable<std::string>::value

// declarations ///////////////////////////////////////////////////////////////////////////////////

namespace ankerl {
namespace nanobench {

using Clock = std::conditional<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock,
                               std::chrono::steady_clock>::type;
class Bench;
struct Config;
class Result;
class Rng;
class BigO;

/**
 * @brief Renders output from a mustache-like template and benchmark results.
 *
 * The templating facility here is heavily inspired by [mustache - logic-less templates](https://mustache.github.io/).
 * It adds a few more features that are necessary to get all of the captured data out of nanobench. Please read the
 * excellent [mustache manual](https://mustache.github.io/mustache.5.html) to see what this is all about.
 *
 * nanobench output has two nested layers, *result* and *measurement*.  Here is a hierarchy of the allowed tags:
 *
 * * `{{#result}}` Marks the begin of the result layer. Whatever comes after this will be instantiated as often as
 *   a benchmark result is available. Within it, you can use these tags:
 *
 *    * `{{title}}` See Bench::title.
 *
 *    * `{{name}}` Benchmark name, usually directly provided with Bench::run, but can also be set with Bench::name.
 *
 *    * `{{unit}}` Unit, e.g. `byte`. Defaults to `op`, see Bench::unit.
 *
 *    * `{{batch}}` Batch size, see Bench::batch.
 *
 *    * `{{complexityN}}` Value used for asymptotic complexity calculation. See Bench::complexityN.
 *
 *    * `{{epochs}}` Number of epochs, see Bench::epochs.
 *
 *    * `{{clockResolution}}` Accuracy of the clock, i.e. what's the smallest time possible to measure with the clock.
 *      For modern systems, this can be around 20 ns. This value is automatically determined by nanobench at the first
 *      benchmark that is run, and used as a static variable throughout the application's runtime.
 *
 *    * `{{clockResolutionMultiple}}` Configuration multiplier for `clockResolution`. See Bench::clockResolutionMultiple.
 *      This is the target runtime for each measurement (epoch). That means the more accurate your clock is, the faster
 *      will be the benchmark. Basing the measurement's runtime on the clock resolution is the main reason why nanobench is so fast.
 *
 *    * `{{maxEpochTime}}` Configuration for a maximum time each measurement (epoch) is allowed to take. Note that at least
 *      a single iteration will be performed, even when that takes longer than maxEpochTime. See Bench::maxEpochTime.
 *
 *    * `{{minEpochTime}}` Minimum epoch time, defaults to 1ms. See Bench::minEpochTime.
 *
 *    * `{{minEpochIterations}}` See Bench::minEpochIterations.
 *
 *    * `{{epochIterations}}` See Bench::epochIterations.
 *
 *    * `{{warmup}}` Number of iterations used before measuring starts. See Bench::warmup.
 *
 *    * `{{relative}}` True or false, depending on the setting you have used. See Bench::relative.
 *
 *    * `{{context(variableName)}}` See Bench::context.
 *
 *    Apart from these tags, it is also possible to use some mathematical operations on the measurement data. The operations
 *    are of the form `{{command(name)}}`.  Currently `name` can be one of `elapsed`, `iterations`. If performance counters
 *    are available (currently only on current Linux systems), you also have `pagefaults`, `cpucycles`,
 *    `contextswitches`, `instructions`, `branchinstructions`, and `branchmisses`. All the measures (except `iterations`) are
 *    provided for a single iteration (so `elapsed` is the time a single iteration took). The following tags are available:
 *
 *    * `{{median(<name>)}}` Calculate median of a measurement data set, e.g. `{{median(elapsed)}}`.
 *
 *    * `{{average(<name>)}}` Average (mean) calculation.
 *
 *    * `{{medianAbsolutePercentError(<name>)}}` Calculates MdAPE, the Median Absolute Percentage Error. The MdAPE is an excellent
 *      metric for the variation of measurements. It is more robust to outliers than the
 *      [Mean absolute percentage error (M-APE)](https://en.wikipedia.org/wiki/Mean_absolute_percentage_error).
 *      @f[
 *       \mathrm{MdAPE}(e) = \mathrm{med}\{| \frac{e_i - \mathrm{med}\{e\}}{e_i}| \}
 *      @f]
 *      E.g. for *elapsed*: First, @f$ \mathrm{med}\{e\} @f$ calculates the median by sorting and then taking the middle element
 *      of all *elapsed* measurements. This is used to calculate the absolute percentage
 *      error to this median for each measurement, as in  @f$ | \frac{e_i - \mathrm{med}\{e\}}{e_i}| @f$. All these results
 *      are sorted, and the middle value is chosen as the median absolute percent error.
 *
 *      This measurement is a bit hard to interpret, but it is very robust against outliers. E.g. a value of 5% means that half of the
 *      measurements deviate less than 5% from the median, and the other deviate more than 5% from the median.
 *
 *    * `{{sum(<name>)}}` Sum of all the measurements. E.g. `{{sum(iterations)}}` will give you the total number of iterations
*        measured in this benchmark.
 *
 *    * `{{minimum(<name>)}}` Minimum of all measurements.
 *
 *    * `{{maximum(<name>)}}` Maximum of all measurements.
 *
 *    * `{{sumProduct(<first>, <second>)}}` Calculates the sum of the products of corresponding measures:
 *      @f[
 *          \mathrm{sumProduct}(a,b) = \sum_{i=1}^{n}a_i\cdot b_i
 *      @f]
 *      E.g. to calculate total runtime of the benchmark, you multiply iterations with elapsed time for each measurement, and
 *      sum these results up:
 *      `{{sumProduct(iterations, elapsed)}}`.
 *
 *    * `{{#measurement}}` To access individual measurement results, open the begin tag for measurements.
 *
 *       * `{{elapsed}}` Average elapsed wall clock time per iteration, in seconds.
 *
 *       * `{{iterations}}` Number of iterations in the measurement. The number of iterations will fluctuate due
 *         to some applied randomness, to enhance accuracy.
 *
 *       * `{{pagefaults}}` Average number of pagefaults per iteration.
 *
 *       * `{{cpucycles}}` Average number of CPU cycles processed per iteration.
 *
 *       * `{{contextswitches}}` Average number of context switches per iteration.
 *
 *       * `{{instructions}}` Average number of retired instructions per iteration.
 *
 *       * `{{branchinstructions}}` Average number of branches executed per iteration.
 *
 *       * `{{branchmisses}}` Average number of branches that were missed per iteration.
 *
 *    * `{{/measurement}}` Ends the measurement tag.
 *
 * * `{{/result}}` Marks the end of the result layer. This is the end marker for the template part that will be instantiated
 *   for each benchmark result.
 *
 *
 *  For the layer tags *result* and *measurement* you additionally can use these special markers:
 *
 *  * ``{{#-first}}`` - Begin marker of a template that will be instantiated *only for the first* entry in the layer. Use is only
 *    allowed between the begin and end marker of the layer. So between ``{{#result}}`` and ``{{/result}}``, or between
 *    ``{{#measurement}}`` and ``{{/measurement}}``. Finish the template with ``{{/-first}}``.
 *
 *  * ``{{^-first}}`` - Begin marker of a template that will be instantiated *for each except the first* entry in the layer. This,
 *    this is basically the inversion of ``{{#-first}}``. Use is only allowed between the begin and end marker of the layer.
 *    So between ``{{#result}}`` and ``{{/result}}``, or between ``{{#measurement}}`` and ``{{/measurement}}``.
 *
 *  * ``{{/-first}}`` - End marker for either ``{{#-first}}`` or ``{{^-first}}``.
 *
 *  * ``{{#-last}}`` - Begin marker of a template that will be instantiated *only for the last* entry in the layer. Use is only
 *    allowed between the begin and end marker of the layer. So between ``{{#result}}`` and ``{{/result}}``, or between
 *    ``{{#measurement}}`` and ``{{/measurement}}``. Finish the template with ``{{/-last}}``.
 *
 *  * ``{{^-last}}`` - Begin marker of a template that will be instantiated *for each except the last* entry in the layer. This,
 *    this is basically the inversion of ``{{#-last}}``. Use is only allowed between the begin and end marker of the layer.
 *    So between ``{{#result}}`` and ``{{/result}}``, or between ``{{#measurement}}`` and ``{{/measurement}}``.
 *
 *  * ``{{/-last}}`` - End marker for either ``{{#-last}}`` or ``{{^-last}}``.
 *
   @verbatim embed:rst

   For an overview of all the possible data you can get out of nanobench, please see the tutorial at :ref:`tutorial-template-json`.

   The templates that ship with nanobench are:

   * :cpp:func:`templates::csv() <ankerl::nanobench::templates::csv()>`
   * :cpp:func:`templates::json() <ankerl::nanobench::templates::json()>`
   * :cpp:func:`templates::htmlBoxplot() <ankerl::nanobench::templates::htmlBoxplot()>`
   * :cpp:func:`templates::pyperf() <ankerl::nanobench::templates::pyperf()>`

   @endverbatim
 *
 * @param mustacheTemplate The template.
 * @param bench Benchmark, containing all the results.
 * @param out Output for the generated output.
 */
void render(char const* mustacheTemplate, Bench const& bench, std::ostream& out);
void render(std::string const& mustacheTemplate, Bench const& bench, std::ostream& out);

/**
 * Same as render(char const* mustacheTemplate, Bench const& bench, std::ostream& out), but for when
 * you only have results available.
 *
 * @param mustacheTemplate The template.
 * @param results All the results to be used for rendering.
 * @param out Output for the generated output.
 */
void render(char const* mustacheTemplate, std::vector<Result> const& results, std::ostream& out);
void render(std::string const& mustacheTemplate, std::vector<Result> const& results, std::ostream& out);

// Contains mustache-like templates
namespace templates {

/*!
  @brief CSV data for the benchmark results.

  Generates a comma-separated values dataset. First line is the header, each following line is a summary of each benchmark run.

  @verbatim embed:rst
  See the tutorial at :ref:`tutorial-template-csv` for an example.
  @endverbatim
 */
char const* csv() noexcept;

/*!
  @brief HTML output that uses plotly to generate an interactive boxplot chart. See the tutorial for an example output.

  The output uses only the elapsed wall clock time, and displays each epoch as a single dot.
  @verbatim embed:rst
  See the tutorial at :ref:`tutorial-template-html` for an example.
  @endverbatim

  @see also ankerl::nanobench::render()
 */
char const* htmlBoxplot() noexcept;

/*!
 @brief Output in pyperf compatible JSON format, which can be used for more analyzation.
 @verbatim embed:rst
 See the tutorial at :ref:`tutorial-template-pyperf` for an example how to further analyze the output.
 @endverbatim
 */
char const* pyperf() noexcept;

/*!
  @brief Template to generate JSON data.

  The generated JSON data contains *all* data that has been generated. All times are as double values, in seconds. The output can get
  quite large.
  @verbatim embed:rst
  See the tutorial at :ref:`tutorial-template-json` for an example.
  @endverbatim
 */
char const* json() noexcept;

