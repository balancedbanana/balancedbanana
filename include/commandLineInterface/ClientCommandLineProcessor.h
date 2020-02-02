#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class ClientCommandLineProcessor : public balancedbanana::commandLineInterface::CommandLineProcessor
{
    public:

        virtual int process(int argc, char** argv, const std::shared_ptr<Task>& task);

    private:

        balancedbanana::configfiles::Priority evaluatePriority(const std::string& priority);
};

} // namespace commandLineInterface
} // namespace balancedbanana