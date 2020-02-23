#pragma once

#include "communication/Task.h"

using balancedbanana::communication::Task;
using balancedbanana::communication::TaskType;


namespace balancedbanana
{
namespace scheduler
{

class ClientRequest
{
public:

    static std::shared_ptr<ClientRequest> selectRequestType(TaskType requestType);

    virtual std::shared_ptr<std::string> executeRequestAndFetchData(const std::shared_ptr<Task>& task);

private:

    ClientRequest();

};

} // namespace scheduler

} // namespace balancedbanana