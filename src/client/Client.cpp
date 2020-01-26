#include <commandLineInterface/CommandLineProcessor.h>
#include <iostream>

using namespace balancedbanana::commandLineInterface;

int main(int argc, char** argv) {
    CommandLineProcessor proc;
    // Wieso argc als Zeiger...
    auto task = proc.process(&argc, argv);
    std::cout << "Task Type" << task->getType() << "\n";
}