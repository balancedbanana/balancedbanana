#pragma once

#include "scheduler/clientRequests/ClientRequest.h"

using balancedbanana::scheduler::ClientRequest;

namespace balancedbanana
{
namespace scheduler
{

class StopRequest : public ClientRequest
{
public:

    std::shared_ptr<std::string> executeRequestAndFetchData(const std::shared_ptr<Task>& task);

private:

};

} // namespace scheduler

} // namespace balancedbanana
