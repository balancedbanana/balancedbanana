#include <worker/Worker.h>

using balancedbanana::worker::Worker;

int main(int argc, char** argv) {

    Worker worker;
    worker.processCommandLineArguments(argc, argv);
    return 0;
}