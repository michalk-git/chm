//$file${.::low_power.h} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: low-power.qm
// File:  ${.::low_power.h}
//
// This code has been generated by QM 4.5.0 (https://www.state-machine.com/qm).
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
//$endhead${.::low_power.h} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#ifndef low_power_h
#define low_power_h

enum BlinkySignals {
    BTN_PRESSED_SIG = QP::Q_USER_SIG, // button SW1 was pressed
    MAX_PUB_SIG,          // the last published signal

    TIMEOUT0_SIG,         // timeout for Blinky0
    TIMEOUT1_SIG,         // timeout for Blinky1
    MAX_SIG               // the last signal
};

//$declare${AOs::AO_Blinky0} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
extern QP::QActive * const AO_Blinky0;
//$enddecl${AOs::AO_Blinky0} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#ifdef qxk_h // QXK kernel?

extern QP::QXThread XT_Blinky1;
extern QP::QXSemaphore XSEM_sw1;

#else // QV or QK kernels

//$declare${AOs::AO_Blinky1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
extern QP::QActive * const AO_Blinky1;
//$enddecl${AOs::AO_Blinky1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif

#endif // low_power_h
