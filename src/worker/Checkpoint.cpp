#include <worker/docker/Checkpoint.h>

using namespace balancedbanana::worker;

Checkpoint::Checkpoint(const std::string & id) {
    this->id = id;
}