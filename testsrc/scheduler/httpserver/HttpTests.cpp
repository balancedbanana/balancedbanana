#include "gtest/gtest.h"
#include <scheduler/httpserver/HttpServer.h>

using namespace balancedbanana::scheduler;

TEST(HttpServer, ApiTest)
{
    HttpServer server;
    server.listen("localhost", 8234);
}