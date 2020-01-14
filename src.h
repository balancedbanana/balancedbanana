#ifndef _SRC_H
#define _SRC_H

#include <string>


class Authenticator : public ClientAuthMessage {
  public:
     authenticate(const std::string & username, const std::string & password);

     authenticate(const std::string & username);

    //Nur f�r worker
     authenticate();

};
class AuthResultMessage : public Message {
  private:
    unsigned long status;


  public:
    //Gibt den Authentfication status zur�ck 0 falls erfolgreich sonst ungleich 0
    unsigned long getStatus();

    virtual void process(const std::shared_ptr<MessageProcessor> & mp);

};
//Authenifiziert einen Arbeiter f�r die erste Verbinung  ( folgend PublicKeyAuthMessage on verwenden)
class ClientAuthMessage : public Message {
  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp);


  private:
    std::string username;

    std::string password;


  public:
    ClientAuthMessage(const std::string & username, const std::string & password, const std::string & pubkey);


  private:
     publickey;

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
class Communicator {
  private:
    SSLSocket * socket;

    MessageProcessor * processor;


  public:
    void listen(const std::function<void(std::shared_ptr<Message>)> & callback);

     send(const Message & message);


  private:
    Worker * ;

};
enum Event {
  Event_StatusRequest,
  Event_TailRequest,
  Event_StopRequest,
  Event_PauseRequest,
  Event_ResumeRequest,
  Event_BackupRequest,
  Event_RestoreRequest
};
class EventDispatcher {
  public:
    void dispatch(const Event & eventType, const std::string & worker, int taskID);

};
class EventHandler {
  public:
     receiveEvent(Event event, int taskID);

};
class Message {
  protected:
    const unsigned int typeId;


  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp) = 0;

    virtual std::string serialize(const  & );

    static std::shared_ptr<Message> deserialize(const const char * & msg, unsigned int size,  );

};
class MessageProcessor {
  private:
    SSLSocket * ;

    Communicator * ;


  public:
    virtual void process(const std::shared_ptr<Message> & msg);

    virtual void processMsgX(const std::shared_ptr<MsgX> & msg);

    virtual void processMsgY(const std::shared_ptr<MsgY> & msg);


  private:
    virtual void handleInvalidMessage(const std::shared_ptr<Message> & msg);

};
class SnapshotMessage : public Message {
  public:
    virtual void process(const std::shared_ptr<MessageProcessor> & mp);


  private:
    unsigned long jobid;

    bool stop;


  public:
    SnapshotMessage(unsigned long tid, bool stop);

};

#endif
