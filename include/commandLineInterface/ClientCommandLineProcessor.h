#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class ClientCommandLineProcessor : public balancedbanana::commandLineInterface::CommandLineProcessor
{
    public:

        virtual std::shared_ptr<Task> process(int argc, char** argv);

    private:

        balancedbanana::configfiles::Priority evaluatePriority(const std::string& priority);
};

} // namespace commandLineInterface
} // namespace balancedbanana