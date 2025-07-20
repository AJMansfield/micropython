#! /usr/bin/env python3

import os
import subprocess
import sys
import argparse
import re
from glob import glob
from collections import defaultdict

import importlib

run_tests_py = importlib.import_module("run-tests")

sys.path.append(run_tests_py.base_path("../tools"))
import pyboard

if os.name == "nt":
    MICROPYTHON = os.getenv(
        "MICROPY_MICROPYTHON", "../ports/windows/build-standard/micropython.exe"
    )
else:
    MICROPYTHON = os.getenv("MICROPY_MICROPYTHON", "../ports/unix/build-standard/micropython")


def run_tests(pyb, test_dict):
    test_count = 0
    testcase_count = 0

    for base_test, tests in sorted(test_dict.items()):
        print(base_test + ":")
        baseline = None
        for test_file in tests:
            # run MicroPython
            if pyb is None:
                # run on PC
                try:
                    output_mupy = subprocess.check_output(
                        [MICROPYTHON, "-X", "emit=bytecode", test_file[0]]
                    )
                except subprocess.CalledProcessError:
                    output_mupy = b"CRASH"
            else:
                # run on pyboard
                pyb.enter_raw_repl()
                try:
                    output_mupy = pyb.execfile(test_file[0]).replace(b"\r\n", b"\n")
                except pyboard.PyboardError:
                    output_mupy = b"CRASH"

            try:
                output_mupy = float(output_mupy.strip())
            except ValueError:
                output_mupy = -1
            test_file[1] = output_mupy
            testcase_count += 1

            if baseline is None:
                baseline = test_file[1]
            print(
                "    %.3fs (%+06.2f%%) %s"
                % (test_file[1], (test_file[1] * 100 / baseline) - 100, test_file[0])
            )

        test_count += 1

    print("{} tests performed ({} individual testcases)".format(test_count, testcase_count))

    # all tests succeeded
    return True


def main():
    cmd_parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description="""Run benchmarks for MicroPython.

By default the tests are run against the unix port of MicroPython. To run it
against something else, use the -t option.  See below for details.
""",
        epilog="""\
The -t option accepts the following for the test instance:
- unix - use the unix port of MicroPython, specified by the MICROPY_MICROPYTHON
  environment variable (which defaults to the standard variant of either the unix
  or windows ports, depending on the host platform)
- webassembly - use the webassembly port of MicroPython, specified by the
  MICROPY_MICROPYTHON_MJS environment variable (which defaults to the standard
  variant of the webassembly port)
- port:<device> - connect to and use the given serial port device
- a<n> - connect to and use /dev/ttyACM<n>
- u<n> - connect to and use /dev/ttyUSB<n>
- c<n> - connect to and use COM<n>
- exec:<command> - execute a command and attach to its stdin/stdout
- execpty:<command> - execute a command and attach to the printed /dev/pts/<n> device
- <a>.<b>.<c>.<d> - connect to the given IPv4 address
- anything else specifies a serial port

Options -i and -e can be multiple and processed in the order given. Regex
"search" (vs "match") operation is used. An action (include/exclude) of
the last matching regex is used:
  run-tests.py -i async - exclude all, then include tests containing "async" anywhere
  run-tests.py -e '/big.+int' - include all, then exclude by regex
  run-tests.py -e async -i async_foo - include all, exclude async, yet still include async_foo
""",
    )
    cmd_parser.add_argument(
        "-t", "--test-instance", default="unix", help="the MicroPython instance to test"
    )
    cmd_parser.add_argument(
        "-b", "--baudrate", default=115200, help="the baud rate of the serial device"
    )
    cmd_parser.add_argument("-u", "--user", default="micro", help="the telnet login username")
    cmd_parser.add_argument("-p", "--password", default="python", help="the telnet login password")
    cmd_parser.add_argument(
        "-d", "--test-dirs", nargs="*", help="input test directories (if no files given)"
    )
    cmd_parser.add_argument(
        "--denominator", type=int, default=1, help="divide input iterations by this value"
    )
    cmd_parser.add_argument("files", nargs="*", help="input test files")
    args = cmd_parser.parse_args()

    # Note pyboard support is copied over from run-tests.py, not tests, and likely needs revamping
    pyb = run_tests_py.get_test_instance(
        args.test_instance, args.baudrate, args.user, args.password
    )

    if len(args.files) == 0:
        if args.test_dirs:
            test_dirs = tuple(args.test_dirs)
        elif pyb is None:
            # run PC tests
            test_dirs = ("internal_bench",)
        else:
            # run pyboard tests
            test_dirs = ("basics", "float", "pyb")

        tests = sorted(
            test_file
            for test_files in (glob("{}/*.py".format(dir)) for dir in test_dirs)
            for test_file in test_files
        )
    else:
        # tests explicitly given
        tests = sorted(args.files)

    test_dict = defaultdict(lambda: [])
    for t in tests:
        m = re.match(r"(.+?)-(.+)\.py", t)
        if not m:
            continue
        test_dict[m.group(1)].append([t, None])

    if pyb is not None:
        print("Uploading bench harness.")
        pyb.fs_put('internal_bench/bench_hw.py', 'bench.py')

    if not run_tests(pyb, test_dict):
        sys.exit(1)


if __name__ == "__main__":
    main()
