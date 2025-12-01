#include "CSymbolGenerator/CCinReaderGenerator/CCinReaderGenerator.hpp"

#include "utilities/conversions.hpp"
#include "utilities/definitions.hpp"
#include <cstring>

#if defined(__unix__)
#include <termios.h>
#include <unistd.h>
#endif

QCSP::CCinReaderGenerator::CCinReaderGenerator()
    : CByteReaderGenerator() {
    byte_stream = &std::cin;
}

void QCSP::CCinReaderGenerator::process(std::vector<uint_gf_t> & symbols) {
#if defined(__unix__)
    struct termios old = {0};
    if (tcgetattr(STDIN_FILENO, &old) < 0) {
        perror("tcsetattr()");
    }
    old.c_lflag &= ~ICANON;
    old.c_cc[VMIN]  = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old) < 0) {
        perror("tcsetattr ICANON");
    }
#endif

    QCSP::CByteReaderGenerator::process(symbols);

#if defined(__unix__)
    old.c_lflag |= ICANON;
    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &old) < 0) {
        perror("tcsetattr ~ICANON");
    }
#endif
}
