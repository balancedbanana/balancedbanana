#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class ClientCommandLineProcessor : public balancedbanana::commandLineInterface::CommandLineProcessor
{
    public:

        int process(int argc, const char* const * argv, const std::shared_ptr<balancedbanana::communication::Task>& task) override;

    private:
};

} // namespace commandLineInterface
} // namespace balancedbanana