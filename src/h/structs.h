#ifndef H_STRUCTS
#define H_STRUCTS

#include "flyff.h"

struct comboItem {
    char *name;
    unsigned long val;
};

struct item_thread_if_connected {
	flyff f;
	void *tab;
	int index;
};

#endif
