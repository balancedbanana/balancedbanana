#include <commandLineInterface/CommandLineProcessor.h>
#include <termios.h>
#include <iostream>

std::string balancedbanana::commandLineInterface::CommandLineProcessor::readPassword() {
    struct termios old, _new;
    bool terminal;
    if((terminal = (tcgetattr (1, &old) == 0))) {
        _new = old;
        _new.c_lflag &= ~ECHO;
        terminal = tcsetattr (1, TCSAFLUSH, &_new) == 0;
    }

    std::string pass;
    /* Read the passphrase */
    std::cin >> pass;

    if(terminal) {
        /* Restore terminal. */
        (void) tcsetattr (1, TCSAFLUSH, &old);
    }
    return pass;
}