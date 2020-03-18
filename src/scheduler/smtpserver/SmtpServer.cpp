#include <scheduler/smtpserver/SmtpServer.h>
#include <Net/TLSSocket.h>
#include <cstdio>
#include <cstring>
#define CRLF "\r\n"                 // carriage-return/line feed pair

using namespace balancedbanana::scheduler;

SmtpServer::SmtpServer(const std::string &mailserver, short port, bool smtps, const std::string &frommail) : mailserver(mailserver), port(port), smtps(smtps), frommail(frommail) {

}

void SmtpServer::sendMail(const std::string & mail, const std::string & subject, const std::string & content) {
  char        szBuffer[4096]       = "";

  auto socket = smtps ? Net::TLSSocket::Connect(mailserver.data(), port, true) : Net::Socket::Connect(mailserver.data(), port);
  if(!socket) {
    throw std::runtime_error("Failed to connect to Mailserver (" + mailserver + ":" + std::to_string(port)));
  }
  auto in = socket->GetInputStream();
  auto out = socket->GetOutputStream();

  // Receive initial response from SMTP server
  if (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0) {
    throw std::runtime_error("Failed to get Server response");
  }

  // Send HELO server.com
  auto helomsg = "HELO " + mailserver + CRLF; 
  if((out.Send((uint8_t*)helomsg.data(), helomsg.length()) != helomsg.length()) || (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0)) {
    throw std::runtime_error("Helo Handshake failed, Last Response from Mailserver: " + std::string(szBuffer));
  }

  // Send MAIL FROM: <sender@mydomain.com>
  auto frommsg = "MAIL FROM:<" + frommail + ">" CRLF; 
  if((out.Send((uint8_t*)frommsg.data(), frommsg.length()) != frommsg.length()) || (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0)) {
    throw std::runtime_error("MAIL FROM failed, Last Response from Mailserver: " + std::string(szBuffer));
  }

  // Send RCPT TO: <receiver@domain.com>
  auto tomsg = "RCPT TO:<" + mail + ">" CRLF; 
  if((out.Send((uint8_t*)tomsg.data(), tomsg.length()) != tomsg.length()) || (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0)) {
    throw std::runtime_error("RCPT TO failed, Last Response from Mailserver: " + std::string(szBuffer));
  }

  const uint8_t datamsg[] = "DATA" CRLF;
  if((out.Send(datamsg, sizeof(datamsg) - 1) != (sizeof(datamsg) - 1)) || (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0)) {
    throw std::runtime_error("Failed to begin Data, Last Response from Mailserver: " + std::string(szBuffer));
  }

  auto _subject = "Subject: " + subject + CRLF;

  if(out.Send((uint8_t*)_subject.data(), _subject.length()) != _subject.length() || out.Send((uint8_t*)content.data(), content.length()) != content.length()) {
    throw std::runtime_error("Failed to send Message Subject / Body, Last Response from Mailserver: " + std::string(szBuffer));
  }

  // Send blank line and a period
  const uint8_t enmsg[] = CRLF "." CRLF;
  if((out.Send(enmsg, sizeof(enmsg) - 1) != (sizeof(enmsg) - 1)) || (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0)) {
    throw std::runtime_error("Failed to Finish Body, Last Response from Mailserver: " + std::string(szBuffer));
  }

  // Send QUIT
  const uint8_t quitmsg[] = "QUIT" CRLF;
  if((out.Send(quitmsg, sizeof(quitmsg) - 1) != (sizeof(quitmsg) - 1)) || (in.Receive((uint8_t*)szBuffer, sizeof(szBuffer)) <= 0)) {
    throw std::runtime_error("Failed to Quit, Last Response from Mailserver: " + std::string(szBuffer));
  }
}
