#include "Specs.h"

//Encapsulates all details required to create Worker object.
struct worker_details {
    //The id of the Worker.
    uint64_t id;


  private:
    std::string name;


  public:
    //The specs of the worker.
    specs specs;

    std::string address;

    std::string public_key;

};