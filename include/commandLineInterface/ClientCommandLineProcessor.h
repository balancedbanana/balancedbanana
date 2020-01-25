#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class ClientCommandLineProcessor : public CommandLineProcessor
{
    public:

        virtual std::shared_ptr<Task> process(int argc, char** argv);

    private:

        Priority evaluatePriority(const std::string& priority);
};

} // namespace commandLineInterface
} // namespace balancedbanana