#ifndef _BBCSRC_H
#define _BBCSRC_H

#include <string>


#include "bb.h"

class Communicator;
class PublicKeyAuthMessage;
class WorkerAuthMessage;
class CommandLineProcessor;

class Authenticator {
  public:
    Authenticator(const Communicator & comm);

     authenticate(const std::string & username, const std::string & password);

     authenticate(const std::string & username);

    //Nur für worker
     authenticate();

};
class AuthResultMessage : public Message {
  private:
    unsigned long status;


  public:
    //Gibt den Authentfication status zurück 0 falls erfolgreich sonst ungleich 0
    unsigned long getStatus();

    virtual void process(const std::shared_ptr<MessageProcessor> & mp);

};
//Authenifiziert einen Arbeiter für die erste Verbinung  ( folgend PublicKeyAuthMessage on verwenden)
class ClientAuthMessage : public Message {
  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp);


  private:
    std::string username;

    std::string password;


  public:
    ClientAuthMessage(const std::string & username, const std::string & password, const std::string & pubkey);


  private:
    std::string publickey;

};
class Client : public Authenticator {
  public:
    //main method
    int main(const char** & argv, const int* & argc);

     snapshot();

};
class ClientMP : public MessageProcessor {
  public:
    virtual void processMsgX(const std::shared_ptr<MsgX> & msg);

};
class Message {
  protected:
    const unsigned int typeId;


  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp) = 0;

    virtual std::string serialize(const  & );

    static std::shared_ptr<Message> deserialize(const const char * & msg, unsigned int size,  );

};
#endif
