@echo off
g++ -o grcalc.exe -DNDEBUG -O3 main.cpp calc.cpp entries.cpp graph.cpp tlist.cpp -lm
