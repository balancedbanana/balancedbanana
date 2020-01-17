#pragma once
#include <string>
#include <functional>

class HttpServer {
  public:
    void listen(const std::string & ip, short port, const std::function<void()> & callback);

    void useSLL(const std::string & privatekeypath, const std::string & certchainpath);

};