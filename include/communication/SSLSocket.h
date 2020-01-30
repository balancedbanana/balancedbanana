// #include <QSslSocket>
#include <string>
#include <functional>
#include <memory>
#include <QtNetwork/QSslSocket>

class SSLSocket {
    QSslSocket socket;
public:
    void connect(const std::string & address);

    void listen(unsigned short port, const std::function<void(std::shared_ptr<SSLSocket>)> & callback);

    void send(const char * msg);

    const char * receive(const char* data, const size_t & size);

    void listen(unsigned short port, const std::function<void()> & callback);
};
