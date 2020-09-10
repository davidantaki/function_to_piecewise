// File: test.cpp
// Author: David Antaki
// Date: 6/22/2020
// License: This software is not open source and is copyrighted by David
// Antaki and Sous Bois International, Inc.
//
// Contents: Simple test file.

#include "FunctionToPiecewise.h"
#include "Printer.h"

// Simple linear function with slope of 2
float Func1(float _x)
{
   return (2 * _x);
}

// Should return exactly the same as the function since
// it's linear
bool TestCase1()
{
   FunctionToPiecewise piecewise(Func1, 1, std::pair<float, float>(0, 5));

   if (piecewise.xToy(1) == 2)
      return true;
   return false;
}

bool TestCase2()
{
   FunctionToPiecewise piecewise(Func1, 1, std::pair<float, float>(0, 5));

   if (piecewise.xToy(0) == 0)
      return true;
   return false;
}

// The equation for the magnetic flux a given distance from a magnet
// @param _d   Distance from magnet
// @return     The flux density at a given distance.
float Func2(float _d)
{
   float l = 19.05;
   float w = 9.525;
   float t = 1.5875;
   float br = 1320;

   return (br / M_PI) *
          (atan((w * l) / (2 * _d * sqrt(4 * pow(_d, 2) + pow(w, 2) + pow(l, 2)))) - atan((w * l) / (2 * (_d + t) * sqrt(4 * pow(_d + t, 2) + pow(w, 2) + pow(l, 2)))));
}

bool TestCase3()
{
   FunctionToPiecewise piecewise(Func2, 100, std::pair<float, float>(0, 16));

   if (piecewise.xToy(14) >= 12.27 && piecewise.xToy(14) <= 12.275 &&
       piecewise.yTox(12.273) >= 13.9 && piecewise.yTox(12.273) <= 14.1)
      return true;
   return false;
}

int main(int argc, char *argv[])
{
   wait(5);
   Printer::pc.printf("TestCase1 returned: %d\n", TestCase1());
   Printer::pc.printf("TestCase2 returned: %d\n", TestCase2());
   Printer::pc.printf("TestCase3 returned: %d\n", TestCase3());

   Printer::pc.printf("Testing complete");
}