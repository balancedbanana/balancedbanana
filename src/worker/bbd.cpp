#include <worker/Worker.h>

using balancedbanana::worker::Worker;

int main(int argc, char** argv) {

    auto worker = std::make_shared<Worker>();
    return worker->processCommandLineArguments(argc, argv).get();
}