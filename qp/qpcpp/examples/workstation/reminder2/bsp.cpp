//****************************************************************************
// Product: Console-based BSP
// Last Updated for Version: 6.3.6
// Date of the Last Update:  2018-10-14
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// Copyright (C) 2002-2018 Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Contact information:
// https://www.state-machine.com
// mailto:info@state-machine.com
//****************************************************************************
#include "qpcpp.h"
#include "bsp.h"

#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace QP;

//............................................................................
void BSP_init(int /*argc*/, char * /*argv*/[]) {
}
//............................................................................
void QF::onStartup(void) {
    QF_setTickRate(BSP_TICKS_PER_SEC, 30); // set the desired tick rate
    QF_consoleSetup();
}
//............................................................................
void QF::onCleanup(void) {
    cout << "\nBye!Bye!\n";
    QF_consoleCleanup();
}
//............................................................................
void QP::QF_onClockTick(void) {
    QF::TICK_X(0U, &l_clock_tick); // perform the QF clock tick processing
    int key = QF_consoleGetKey();
    if (key != 0) { /* any key pressed? */
        BSP_onKeyboardInput((uint8_t)key);
    }
}
//............................................................................
extern "C" void Q_onAssert(char_t const * const file, int_t line) {
    cerr << "Assertion failed in " << file << " line " << line << endl;
    QF::onCleanup();
    exit(-1);
}
