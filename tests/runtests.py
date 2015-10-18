# A simple test runner for slgc.
#
# Each test file can be divided into several blocks. Three blocks are manditory:
#   TEST: Contains the test name and required configurations.
#   FILE: Contains the code being run.
#   EXPECT: Contains the expectations.
#

import sys
import os
import re
import tempfile
import subprocess

from glob import glob


class TestRunner:

    def __init__(self):
        self.slgc_bin  = os.path.realpath(os.path.dirname(__file__) + "/../slgc")
        self.slgvm_bin = os.path.realpath(os.path.dirname(__file__) + "/../slgvm")

class Test:

    def __init__(self):
        self.file = ""
        self.expect = ""

def find_tests():
    tests = []
    tests_dir = os.path.realpath(os.path.dirname(__file__) + "/functional")

    os.chdir(tests_dir)

    for file in os.listdir(tests_dir):
        if file.endswith(".slt"):
            tests.append(file)

    return tests

def parse_test(file):
    with open(file) as file_handle:
        content = file_handle.readlines()

    state = "INIT"
    test  = Test()

    section_pattern = re.compile("%%([A-Z]*)%%")

    for line in content:
        m = section_pattern.match(line)

        if m is not None:
            section_name = m.group(1)
            if (section_name == "TEST"):
                state = "TEST_SECTION"
            elif (section_name == "FILE"):
                state = "FILE_SECTION"
            elif (section_name == "EXPECT"):
                state = "EXPECT_SECTION"
            else:
                pass
                #raise InputError(line, "Unknown section {0}" % section_name)
        else:
            if (state == "TEST_SECTION"):
                pass
            elif (state == "FILE_SECTION"):
                test.file += line
            elif (state == "EXPECT_SECTION"):
                test.expect += line
            else:
                pass
                #raise InputError(line, "Expected section beginning")

    return test


def run_test(test, tmpdir):
    tmpfile = tempfile.NamedTemporaryFile()
    tmpfile.write(test.file)

    print(test.expect)


slgc_bin = os.path.realpath(os.path.dirname(__file__) + "/../slgc")

test_files = find_tests()
tmpdir = tempfile.TemporaryDirectory(dir=tempfile.gettempdir())

for file in test_files:
    test = parse_test(file)
    run_test(test, tmpdir)
