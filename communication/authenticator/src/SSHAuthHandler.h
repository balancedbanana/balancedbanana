#include <AuthHandler.h>

class SSHAuthHandler : public AuthHandler {
  public:
     authenticate(const std::string & username, const std::string & password);

};