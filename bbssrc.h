#ifndef _BBSSRC_H
#define _BBSSRC_H


#include <string>

#include "bb.h"

class AuthResultMessage;
class JobConfig;
struct job_result;
class Timer;
class Message;
class Communicator;
class EventDispatcher;
class TaskMessage;

class AuthHandler {
  public:
    static AuthHandler GetDefault(const  & );

    AuthHandler(const  & );

    virtual  authenticate(const std::shared_ptr<IUser> & user, const std::string & password) = 0;

     authenticate(const std::shared_ptr<IUser> & user, const std::string & signature);

};

#endif
