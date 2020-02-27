#include "gtest/gtest.h"
#include <scheduler/smtpserver/SmtpServer.h>

using namespace balancedbanana::scheduler;

TEST(SmtpTests, sendmail)
{
    SmtpServer server("localhost", 25, false, "balancedbanana@localhost");
    server.sendMail("steve@localhost", "Balancedbanana Test", "Hallo Welt");
}