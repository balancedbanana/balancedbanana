#include <Communicator.h>

class Worker : public IUser {
public:
    void send(const Message & msg);

    void getStatus();

    void getSpec();
private:
    std::shared_ptr<Communicator> comm;
};