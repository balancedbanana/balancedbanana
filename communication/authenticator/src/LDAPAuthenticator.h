#include <AuthHandler>

class LDAPAuthHandler : public AuthHandler {
  public:
     authenticate(const std::string & username, const std::string & password);

};