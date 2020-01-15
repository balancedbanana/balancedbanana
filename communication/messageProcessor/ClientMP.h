#ifndef CLIENTMP_H
#define CLIENTMP_H


class ClientMP : public MessageProcessor {
public:
	virtual void processMsgX(const std::shared_ptr<MsgX>& msg);

};


#endif // !CLIENTMP_H
