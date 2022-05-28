#include "Exporter.h"
#include "Screen.h"
#include "Turtle.h"

NativeClass* getClass_0()
{
	return new MyScreen;
}

const char* getClassName_0()
{
	return "Screen";
}

NativeClass* getClass_1()
{
	return new MyTurtle;
}

const char* getClassName_1()
{
	return "Turtle";
}