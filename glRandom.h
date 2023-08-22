#pragma once

#include <cstdlib>

class glRandom
{
public:
	static float RandFloat() { return ((rand() * 2.0f) / (RAND_MAX + 1.0f)); }
	static int RandInt(int x, int y) { return rand() % (y - x + 1) + x; }
	static bool RandBool() { if (RandFloat() > 0.5) return true;  else return false; }

};