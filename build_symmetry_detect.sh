#!/bin/bash
# This script builds the 'symmetry_detect' executable, a stand-alone tool for detecting symmetries in ASTs.
# This can be built with the Makefile, this script is provided for users who do not want to install GAP.

c++ YAPB++/simple_graph/symmetry_detect.cc YAPB++/simple_graph/symmetry_parse.cc YAPB++/simple_graph/gason/gason.cpp YAPB++/simple_graph/simple_graph.cc YAPB++/simple_graph/simple_parser.cc -I YAPB++/source -o symmetry_detect -O3 -std=c++14 -I gap_cpp_headers
