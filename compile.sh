#!/bin/bash

g++ -std=c++17 $(root-config --cflags) $(root-config --libs) eff-mc.cxx -o eff-mc
g++ -std=c++17 $(root-config --cflags) $(root-config --libs) extraction-kr85.cxx -o extraction-kr85
