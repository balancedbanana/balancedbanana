#include <commandLineInterface/CommandLineProcessor.h>
#include <termios.h>
#include <iostream>

std::string balancedbanana::commandLineInterface::CommandLineProcessor::readPassword() {
    struct termios old, _new;
    if (tcgetattr (1, &old) != 0)
        throw std::runtime_error("Failed to read password");
    _new = old;
    _new.c_lflag &= ~ECHO;
    if (tcsetattr (1, TCSAFLUSH, &_new) != 0)
        throw std::runtime_error("Failed to read password");

    std::string pass;
    /* Read the passphrase */
    std::cin >> pass;

    /* Restore terminal. */
    (void) tcsetattr (1, TCSAFLUSH, &old);
    return pass;
}