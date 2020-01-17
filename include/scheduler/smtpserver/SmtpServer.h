#pragma once

class SmtpServer {
public:
    void sendMail(const std::string & mail, const std::string & subject, const std::string & content);

};