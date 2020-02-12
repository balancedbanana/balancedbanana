#pragma once


#include "CommandLineProcessor.h"

namespace balancedbanana
{
namespace commandLineInterface
{
    
class SchedulerCommandLineProcessor : public balancedbanana::commandLineInterface::CommandLineProcessor
{
    public:

        virtual int process(int argc, char** argv, const std::shared_ptr<communication::Task>& task);

    private:


};

} // namespace commandLineInterface
} // namespace balancedbanana