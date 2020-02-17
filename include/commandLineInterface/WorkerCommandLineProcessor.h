#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class WorkerCommandLineProcessor : public balancedbanana::commandLineInterface::CommandLineProcessor
{
    public:

        int process(int argc, char** argv, const std::shared_ptr<balancedbanana::communication::Task>& task) override;

    private:


};

} // namespace commandLineInterface
} // namespace balancedbanana