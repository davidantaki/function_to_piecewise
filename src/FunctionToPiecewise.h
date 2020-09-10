// File: FunctionToPiecewise.h
// Author: David Antaki
// Date: 5/25/2020
// License: This software is not open source and is copyrighted by David
// Antaki and Sous Bois International, Inc.
//
// Contents: Holds a piecewise function with n segments that represents
// the standard equation for calculating the magnetic flux density of a
// block magnet at a given distance. What we want is to solve this equation
// for the distance, therefore giving the distance as a function of the
// flux density measured by the hall sensor, however, this function cannot
// be easily solved for distance, therefore a piecewise function is used.
// The function can be found in the DRV5056 datasheet and at:
// https://www.ti.com/lit/ds/symlink/drv5056.pdf?ts=1590447553564

#ifndef FUNCTION_TO_PIECWISE_H
#define FUNCTION_TO_PIECWISE_H

#include <map>
#include <iterator>
#include "mbed.h"
#include "Printer.h"

class FunctionToPiecewise
{
public:
    // @param float (*function)(float)  A function pointer that represents a function
    //                                  that this piecewise function will represent.
    // @param _nSegments    The number of linear piecewise functions to slice
    //                      the passed function into.
    // @param _interval     The interval of the passed function to be converted
    //                      into a piecewise function.
    FunctionToPiecewise(float (*function)(float), int _nSegments, std::pair<float, float> _interval);
    virtual ~FunctionToPiecewise();

    // Takes an x value and returns y.
    //
    // @param _x    The x value to be inputted into the piecewise function
    //              to get a y-value out.
    // @return      The y value of the function.
    float xToy(float _x);

    // Takes a y value and returns x.
    //
    // @param _y    The x value to be inputted into the piecewise function
    //              to get a y-value out.
    // @return      The x value of the function.
    float yTox(float _y);

private:
    // A linear function represented by its slope and y-intercept
    typedef struct
    {
        float slope;
        float yint;
    } LineFunc;

    // A point on the xy plane
    typedef struct
    {
        float x;
        float y;
    } Point;

    // Holds the function that this piecewise represents.
    // This function was passed in through the constructor
    float (*originalFunciton)(float);

    // Holds interval along the x-axis and the f(x) Function
    std::map<std::pair<float, float>, LineFunc> f_of_x_fns;

    // Holds interval along the x-axis and the f(y) Function
    std::map<std::pair<float, float>, LineFunc> f_of_y_fns;

    // Evaluates a linear function given the linear function and an x-value
    //
    // @param _x    The x-value to be inputted into the linear function
    // @param _line The linear function that is to be evaluated for x.
    float evalLinearFunction(float _x, LineFunc _line);

    // Returns the slope of a linear function given 2 points on that function
    //
    // @param _pt1  First point
    // @param _pt2  Second Point
    // @return      Slope
    float getLineFuncSlope(Point _pt1, Point _pt2);

    // Returns the y-intercept of a linear function given 2 points on that function
    // b = y - mx
    //
    // @param _pt1  First point
    // @param _pt2  Second point
    // @return      y-intercept
    float getLineFuncYInt(Point _pt1, Point _pt2);
};

#endif //FUNCTION_TO_PIECWISE_H

FunctionToPiecewise::FunctionToPiecewise(float (*function)(float), int _nSegments, std::pair<float, float> _interval)
{
    // Store the passed function in member variable
    originalFunciton = function;

    // The "subinterval" of the full interval.
    // In other words, if we want to break the below function into 10 segments
    // from x=0 to x=10, than the increment would be 1 and the first segment
    // would be x=0 to x=1, the next segment would be x=1 to x=2, and so on.
    //
    // y
    // |
    // |                  ___
    // |\    ____        /
    // | \__/    \      /
    // |          \____/
    // |_____________________ x
    // | | | | | | | | | | |
    // 0 1 2 3 4 5 6 7 8 9 10
    //
    float xIncrement = (_interval.second - _interval.first) / _nSegments;

    // ---------------------------------------------------------------------

    // Set the f_of_x_fns' intervals and functions
    for (float x = _interval.first; x < _interval.second; x += xIncrement)
    {
        Point tempPt1;
        Point tempPt2;
        LineFunc tempLineFunc;
        std::pair<float, float> tempSubInterval;

        tempPt1.x = x;
        tempPt1.y = (*function)(tempPt1.x);

        tempPt2.x = x + xIncrement;
        tempPt2.y = (*function)(tempPt2.x);

        tempLineFunc.slope = getLineFuncSlope(tempPt1, tempPt2);
        tempLineFunc.yint = getLineFuncYInt(tempPt1, tempPt2);

        // Assigns the subinterval, BUT insures that the second number in the
        // interval (tempSubinterval.second) is always greater than the first
        // number (tempSubinterval.first). The second # must be greater
        // or else the check for if a number is within the interval (this
        // check occurs in the "xToy" and "yTox" intervals) will fail.
        if (tempPt1.y > tempPt2.y)
        {
            tempSubInterval.first = tempPt2.y;
            tempSubInterval.second = tempPt1.y;
        }
        else if (tempPt2.y > tempPt1.y)
        {
            tempSubInterval.first = tempPt1.y;
            tempSubInterval.second = tempPt2.y;
        }

        tempSubInterval.first = tempPt1.x;
        tempSubInterval.second = tempPt2.x;

        f_of_x_fns.insert(std::make_pair(tempSubInterval, tempLineFunc));
    }

    //------------------------------------------------------------------------

    // Set the f_of_y_fns' intervals and functions
    // The for loop is still in terms of x because if it were in terms of y,
    // we would have to evaluate the passed (*function) pointer for y,
    // which we can't do; we can only evaluate it for x.
    for (float x = _interval.first; x < _interval.second; x += xIncrement)
    {
        Point tempPt1;
        Point tempPt2;
        LineFunc tempLineFunc;
        std::pair<float, float> tempSubInterval;

        tempPt1.x = x;
        tempPt1.y = (*function)(tempPt1.x);

        tempPt2.x = x + xIncrement;
        tempPt2.y = (*function)(tempPt2.x);

        // getLineFuncSlope() will give us the f(x) slope "m". We want the
        // f(y) slope which is 1/m
        // y=mx+b --> x=(y-b)/m  or  x=(y/m)-(b/m)
        tempLineFunc.slope = 1 / getLineFuncSlope(tempPt1, tempPt2);

        // getLineFuncYInt() will give us the f(x) y-int "b". We want the
        // f(y) y-int which is -b/x
        // y=mx+b --> x=(y-b)/m  or  x=(y/m)-(b/m)
        tempLineFunc.yint = (-1 * getLineFuncYInt(tempPt1, tempPt2)) / getLineFuncSlope(tempPt1, tempPt2);

        // Assigns the subinterval, BUT insures that the second number in the
        // interval (tempSubinterval.second) is always greater than the first
        // number (tempSubinterval.first). The second # must be greater
        // or else the check for if a number is within the interval (this
        // check occurs in the "xToy" and "yTox" intervals) will fail.
        if (tempPt1.y > tempPt2.y)
        {
            tempSubInterval.first = tempPt2.y;
            tempSubInterval.second = tempPt1.y;
        }
        else if (tempPt2.y > tempPt1.y)
        {
            tempSubInterval.first = tempPt1.y;
            tempSubInterval.second = tempPt2.y;
        }

        // Printer::pc.printf("tempSubInterval.first: %f\n", tempSubInterval.first);
        // Printer::pc.printf("tempSubInterval.second: %f\n", tempSubInterval.second);

        f_of_y_fns.insert(std::make_pair(tempSubInterval, tempLineFunc));
    }
}

FunctionToPiecewise::~FunctionToPiecewise()
{
}

float FunctionToPiecewise::xToy(float _x)
{
    auto iter = std::find_if(f_of_x_fns.cbegin(), f_of_x_fns.cend(),
                             // [=] means pass any variables by value to the lambda
                             [=](const std::pair<std::pair<float, float>, LineFunc> &fn) {
                                 return _x >= fn.first.first && _x < fn.first.second;
                             });

    // If the _x value is out of range of the piecewise, throw error
    if (iter == f_of_x_fns.end())
    {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "_x value is out of the piecewise function's interval");
    }

    return evalLinearFunction(_x, iter->second);
}

float FunctionToPiecewise::yTox(float _y)
{
    auto iter = std::find_if(f_of_y_fns.cbegin(), f_of_y_fns.cend(),
                             // [=] means pass any variables by value to the lambda
                             [=](const std::pair<std::pair<float, float>, LineFunc> &fn) {
                                 return _y >= fn.first.first && _y < fn.first.second;
                             });

        // Printer::pc.printf("%f\n", f_of_y_fns);
    // Printer::pc.printf("%f\n", iter->first.first);
    // Printer::pc.printf("%f\n", iter->first.second);
    // Printer::pc.printf("%f\n", iter->second);

    // If the _x value is out of range of the piecewise, throw error
    if (iter == f_of_y_fns.end())
    {
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "_y value is out of the piecewise function's range");
    }

    // Uses the opposite of y=mx+b --> x = (y-b)/x
    return evalLinearFunction(_y, iter->second);
}

// float FunctionToPiecewise::yTox(float _y)
// {
//     int i;
//     auto iter = std::find_if(f_of_x_fns.cbegin(), f_of_x_fns.cend(),
//                              // [=] means pass any variables by value to the lambda
//                              [=](const std::pair<std::pair<float, float>, LineFunc> &fn) {
//                                  return _y >= evalLinearFunction(fn.first.first, fn.second) && _y < evalLinearFunction(fn.first.second, fn.second);
//                              });

//     // If the _x value is out of range of the piecewise, throw error
//     if (iter == f_of_x_fns.end())
//     {
//         MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "_y value is out of the piecewise function's range");
//     }

//     // Uses the opposite of y=mx+b --> x = (y-b)/x
//     return (_y - iter->second.yint) / iter->second.slope;
// }

float FunctionToPiecewise::evalLinearFunction(float _x, LineFunc _line)
{
    return (_line.slope * _x) + _line.yint;
}

float FunctionToPiecewise::getLineFuncSlope(Point _pt1, Point _pt2)
{
    return (_pt2.y - _pt1.y) / (_pt2.x - _pt1.x);
}

float FunctionToPiecewise::getLineFuncYInt(Point _pt1, Point _pt2)
{
    return _pt1.y - (getLineFuncSlope(_pt1, _pt2) * _pt1.x);
}