#ifndef SUMMONER_H
#define SUMMONER_H

#include <windows.h>
#include <vector>

// search.cpp
unsigned long get_module(unsigned long pid, char *module_name, unsigned long *size = 0);
std::vector<unsigned long> get_procs(char *name);
HWND find_main_window(unsigned long process_id);

#endif
