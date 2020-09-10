// File: Printer.h
// Author: David Antaki
// Date: 5/27/2020
// License: Closed source
// Contents: A generic printer hcear for

#ifndef PRINTER_H
#define PRINTER_H

#include "mbed.h"

class Printer
{
public:
    static Serial pc;
    Printer(); //: pc(USBTX, USBRX, BAUDRATE){};
};

Serial Printer::pc(USBTX, USBRX, 9600);

#endif //PRINTER_H
