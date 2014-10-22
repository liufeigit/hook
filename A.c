#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
// libA.dylib

typedef struct A{
	int num;
	char *name;
	void (*callBack)(int );
	void *id;
} A;

void testA();

void callBack(int );

A *initA();
