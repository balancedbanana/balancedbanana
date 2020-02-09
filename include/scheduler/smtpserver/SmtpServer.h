#pragma once
#include <string>

class SmtpServer {
    std::string mailserver;
    short port;
    bool smtps;
    std::string frommail;
public:
    SmtpServer(const std::string & mailserver, short port, bool smtps, const std::string & frommail);

    void sendMail(const std::string & mail, const std::string & subject, const std::string & content);

};