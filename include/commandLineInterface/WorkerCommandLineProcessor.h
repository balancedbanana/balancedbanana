#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class WorkerCommandLineProcessor : public CommandLineProcessor
{
    public:

        virtual std::shared_ptr<Task> process(int argc, char** argv);

    private:


};

} // namespace commandLineInterface
} // namespace balancedbanana