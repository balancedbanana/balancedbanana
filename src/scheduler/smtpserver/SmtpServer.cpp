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
  char        szLine[255]          = "";
  char        szMsgLine[255]       = "";

  auto socket = smtps ? Net::TLSSocket::Connect(mailserver.data(), port, true) : Net::Socket::Connect(mailserver.data(), port);
  auto in = socket->GetInputStream();
  auto out = socket->GetOutputStream();

  // Receive initial response from SMTP server
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

  // Send HELO server.com
  sprintf(szMsgLine, "HELO %s" CRLF, mailserver.data());
  out.Send((uint8_t*)szMsgLine, strlen(szMsgLine));
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

  // Send MAIL FROM: <sender@mydomain.com>
  sprintf(szMsgLine, "MAIL FROM:<%s>" CRLF, frommail.data());
  out.Send((uint8_t*)szMsgLine, strlen(szMsgLine));
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

  // Send RCPT TO: <receiver@domain.com>
  sprintf(szMsgLine, "RCPT TO:<%s>" CRLF, mail.data());
  out.Send((uint8_t*)szMsgLine, strlen(szMsgLine));
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

  // Send DATA
  sprintf(szMsgLine, "DATA" CRLF);
  out.Send((uint8_t*)szMsgLine, strlen(szMsgLine));
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

  auto _subject = "Subject: " + subject + CRLF;
  out.Send((uint8_t*)_subject.data(), _subject.length());

  out.Send((uint8_t*)content.data(), content.length());


  // Send blank line and a period
  sprintf(szMsgLine, CRLF "." CRLF);
  out.Send((uint8_t*)szMsgLine, strlen(szMsgLine));
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

  // Send QUIT
  sprintf(szMsgLine, "QUIT" CRLF);
  out.Send((uint8_t*)szMsgLine, strlen(szMsgLine));
  in.Receive((uint8_t*)szBuffer, sizeof(szBuffer));

}
