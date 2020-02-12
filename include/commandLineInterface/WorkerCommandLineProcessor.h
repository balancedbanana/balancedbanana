#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class WorkerCommandLineProcessor : public balancedbanana::commandLineInterface::CommandLineProcessor
{
    public:

        int process(int argc, const char*const* argv, const std::shared_ptr<communication::Task>& task) override;

    private:


};

} // namespace commandLineInterface
} // namespace balancedbanana