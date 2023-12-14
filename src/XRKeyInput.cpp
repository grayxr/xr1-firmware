#include <XRKeyInput.h>
#include <map>

namespace XRKeyInput
{
    const int INPUT_LENGTH_MAX = 28;

    const int ALPHA_KEYSET_SIZE = 26;
    const int NUMERIC_KEYSET_SIZE = 10;
    const int SYMBOL_KEYSET_SIZE = 10;

    const char *alphaKeyset[ALPHA_KEYSET_SIZE] = {
        "A", "B", "C", "D", "E", "F", "G", "H",
        "I", "J", "K", "L", "M", "N", "O", "P",
        "Q", "R", "S", "T", "U", "V", "W", "X",
        "Y", "Z"};

    const char *numericKeyset[NUMERIC_KEYSET_SIZE] = {
        "0", "1", "2", "3", "4", "5", "6", "7",
        "8", "9"};

    const char *symbolKeyset[SYMBOL_KEYSET_SIZE] = {
        "!", "-", "_", "(", ")", "#", "&", "$",
        "*", "%"};

    std::map<TEXT_KEYS, uint8_t> textKeyElementToKey = {
        {ELEM1, 0},
        {ELEM2, 1},
        {ELEM3, 2},
        {ELEM4, 3},
        {ELEM5, 4},
        {ELEM6, 5},
        {ELEM7, 6},
        {ELEM8, 7},
    };

    std::string _inputBuf = "";
    uint8_t _currentKeyRow = 0;
    uint8_t _currentCursorIndex = 0;
    TEXT_KEYSET _currentKeyset = TEXT_KEYSET::UPPER_ALPHA;

    void enter(std::string input, int index)
    {
        if (_inputBuf.length() == INPUT_LENGTH_MAX)
        {
            return;
        }

        if (index < 0)
        {
            // no index provided, just append to the string
            _inputBuf += input;
        }
        else
        {
            std::string tempInputBuf = _inputBuf;
            tempInputBuf.insert(index, input);
            _inputBuf = tempInputBuf;
        }
    }

    void enterForElement(TEXT_KEYS element)
    {
        std::string input;
        int8_t elementToKey = textKeyElementToKey[element];
        int8_t elementAtRow = elementToKey + (8 * _currentKeyRow);

        if (_currentKeyset == TEXT_KEYSET::UPPER_ALPHA)
        {
            if (elementAtRow > (ALPHA_KEYSET_SIZE - 1))
            {
                return;
            }

            input = alphaKeyset[elementAtRow];
        }
        else if (_currentKeyset == TEXT_KEYSET::LOWER_ALPHA)
        {
            if (elementAtRow > (ALPHA_KEYSET_SIZE - 1))
            {
                return;
            }

            input = alphaKeyset[elementAtRow];

            for (auto &c : input)
            {
                c = tolower(c);
            }
        }
        else if (_currentKeyset == TEXT_KEYSET::NUMERIC)
        {
            if (elementAtRow > (NUMERIC_KEYSET_SIZE - 1))
            {
                return;
            }

            input = numericKeyset[elementAtRow];
        }
        else if (_currentKeyset == TEXT_KEYSET::SYMBOLS)
        {
            if (elementAtRow > (SYMBOL_KEYSET_SIZE - 1))
            {
                return;
            }

            input = symbolKeyset[elementAtRow];
        }

        int cursorPosToUse = -1;

        // todo: maybe allow for moving cursor?
        // if ((_inputBuf.length()-1) > _currentCursorIndex) {
        //     cursorPosToUse = _currentCursorIndex;
        // }

        // just try always incrementing it when entring new input
        ++_currentCursorIndex;

        enter(input, cursorPosToUse);
    }

    void selectNextKeyRow()
    {
        if (_currentKeyset == TEXT_KEYSET::UPPER_ALPHA || _currentKeyset == TEXT_KEYSET::LOWER_ALPHA)
        {
            ++_currentKeyRow;
            _currentKeyRow = constrain(_currentKeyRow, 0, 3);
        }
        else
        {
            ++_currentKeyRow;
            _currentKeyRow = constrain(_currentKeyRow, 0, 1);
        }
    }

    void selectPreviousKeyRow()
    {
        if (_currentKeyset == TEXT_KEYSET::UPPER_ALPHA || _currentKeyset == TEXT_KEYSET::LOWER_ALPHA)
        {
            --_currentKeyRow;
            _currentKeyRow = constrain(_currentKeyRow, 0, 3);
        }
        else
        {
            --_currentKeyRow;
            _currentKeyRow = constrain(_currentKeyRow, 0, 1);
        }
    }

    void selectAlphaKeyset()
    {
        _currentKeyRow = 0;
        _currentKeyset = TEXT_KEYSET::UPPER_ALPHA;
    }

    void selectNumericKeyset()
    {
        _currentKeyRow = 0;
        _currentKeyset = TEXT_KEYSET::NUMERIC;
    }

    void selectSymbolKeyset()
    {
        _currentKeyRow = 0;
        _currentKeyset = TEXT_KEYSET::SYMBOLS;
    }

    void toggleAlphaCase()
    {
        if (_currentKeyset == TEXT_KEYSET::LOWER_ALPHA || (_currentKeyset != TEXT_KEYSET::LOWER_ALPHA && _currentKeyset != TEXT_KEYSET::UPPER_ALPHA))
        {
            _currentKeyset = TEXT_KEYSET::UPPER_ALPHA;
        }
        else if (_currentKeyset == TEXT_KEYSET::UPPER_ALPHA)
        {
            _currentKeyset = TEXT_KEYSET::LOWER_ALPHA;
        }
    }

    void remove()
    {
        if (_inputBuf.length() == 0)
        {
            return;
        }

        if (_inputBuf.length() == 1)
        {
            _inputBuf = "";
            return;
        }

        std::string tempInputBuf = _inputBuf;
        tempInputBuf.erase(tempInputBuf.length() - 1, 1);
        _inputBuf = tempInputBuf;
    }

    void reset()
    {
        _inputBuf = "";
    }

    std::string get()
    {
        return _inputBuf;
    }

    std::string getKeyForCurrentRow(int idx)
    {
        std::string result;

        int8_t elementAtRow = idx + (8 * _currentKeyRow);

        if (_currentKeyset == TEXT_KEYSET::UPPER_ALPHA)
        {
            if (elementAtRow > (ALPHA_KEYSET_SIZE - 1))
            {
                return result;
            }

            result = alphaKeyset[elementAtRow];
        }
        else if (_currentKeyset == TEXT_KEYSET::LOWER_ALPHA)
        {
            if (elementAtRow > (ALPHA_KEYSET_SIZE - 1))
            {
                return result;
            }

            result = alphaKeyset[elementAtRow];

            for (auto &c : result)
            {
                c = tolower(c);
            }
        }
        else if (_currentKeyset == TEXT_KEYSET::NUMERIC)
        {
            if (elementAtRow > (NUMERIC_KEYSET_SIZE - 1))
            {
                return result;
            }

            result = numericKeyset[elementAtRow];
        }
        else if (_currentKeyset == TEXT_KEYSET::SYMBOLS)
        {
            if (elementAtRow > (SYMBOL_KEYSET_SIZE - 1))
            {
                return result;
            }

            result = symbolKeyset[elementAtRow];
        }

        return result;
    }

    TEXT_KEYSET getCurrentKeyset()
    {
        return _currentKeyset;
    }
}