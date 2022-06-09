#!/bin/bash

g++ -std=c++17 $(root-config --cflags) $(root-config --libs) mc-combo.cxx -o mc-combo
g++ -std=c++17 $(root-config --cflags) $(root-config --libs) extraction-kr85.cxx -o extraction-kr85
