// Copyright 2020 Josh Bailey (josh@vandervecken.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <FastGPIO.h>
#include <MIDI.h>

const byte D2 = 2;
const byte D3 = 3;
const byte D4 = 4;
const byte D5 = 5;
const byte D9 = 9;
const byte pins = 5;
const byte midiChannel = 11;

#define PINNAME(F, P) F##P
#define SETPINHIGH(P) PINNAME(setPinHigh, P)
#define SETPINLOW(P)  PINNAME(setPinLow, P)
#define SETPINOUT(P)  PINNAME(setPinOut, P)
#define SETPIN(P) \
  inline void SETPINHIGH(P)() {FastGPIO::Pin<P>::setOutputValueHigh();} \
  inline void SETPINLOW(P)() {FastGPIO::Pin<P>::setOutputValueLow();} \
  inline void SETPINOUT(P)() {FastGPIO::Pin<P>::setOutputLow();}

SETPIN(D2) SETPIN(D3) SETPIN(D4) SETPIN(D5) SETPIN(D9)
#define PINFUNC(F) F(D2), F(D3), F(D4), F(D5), F(D9)

typedef void (*fp)(void);

fp setPinOut[] = {
  PINFUNC(SETPINOUT),
};

fp setPinHigh[] = {
  PINFUNC(SETPINHIGH),
};

fp setPinLow[] = {
  PINFUNC(SETPINLOW),
};


MIDI_CREATE_DEFAULT_INSTANCE();

inline void handleNoteOn(byte channel, byte note, byte velocity) {
  if (note < pins) {
    setPinHigh[note]();
  }
}

inline void handleNoteOff(byte channel, byte note, byte velocity) {
  if (note < pins) {
    setPinLow[note]();
  }
}

inline void handleResetAll() {
  for (byte note = 0; note < pins; ++note) {
    setPinOut[note]();
  }
}

void setup() {
  handleResetAll();
  MIDI.begin(midiChannel);
  MIDI.turnThruOff();
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleSystemReset(handleResetAll);
}

void loop() {
  MIDI.read();
}
