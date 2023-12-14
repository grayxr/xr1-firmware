#ifndef XRKeyInput_h
#define XRKeyInput_h

#include <Arduino.h>
#include <string>

namespace XRKeyInput
{
    enum TEXT_KEYSET
    {
        UPPER_ALPHA = 0,
        LOWER_ALPHA,
        NUMERIC,
        SYMBOLS
    };

    enum TEXT_KEYS
    {
        ELEM1 = 0,
        LEFT_ARROW = 1,
        ELEM2 = 2,
        RIGHT_ARROW = 3,
        ELEM3 = 4,
        ELEM4 = 5,
        CASE_TOGGLE = 6,
        ELEM5 = 7,
        NUMBERS_SELECT = 8,
        ELEM6 = 9,
        SYMBOLS_SELECT = 10,
        ELEM7 = 11,
        ELEM8 = 12,
    };

    extern const char *alphaKeyset[26];
    extern const char *numericKeyset[10];
    extern const char *symbolKeyset[10];

    extern std::string _inputBuf;
    extern uint8_t _currentKeyRow;
    extern uint8_t _currentCursorIndex;
    extern TEXT_KEYSET _currentKeyset;

    void enter(std::string input, int index = -1);
    void enterForElement(TEXT_KEYS element);
    void remove();
    void reset();

    void selectNextKeyRow();
    void selectPreviousKeyRow();
    void selectAlphaKeyset();
    void selectNumericKeyset();
    void selectSymbolKeyset();
    void toggleAlphaCase();

    TEXT_KEYSET getCurrentKeyset();

    std::string get();
    std::string getKeyForCurrentRow(int idx);
}

#endif /* XRKeyInput_h */