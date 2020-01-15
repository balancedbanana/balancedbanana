#ifndef CLIENTAUTHMESSAGE_H
#define CLIENTAUTHMESSAGE_H


//Authenifiziert einen Arbeiter für die erste Verbinung  ( folgend PublicKeyAuthMessage on verwenden)
class ClientAuthMessage : public Message {
public:
	virtual void process(const std::shared_ptr<MessageProcessor>& mp);


private:
	std::string username;

	std::string password;


public:
	ClientAuthMessage(const std::string& username, const std::string& password, const std::string& pubkey);


private:
	std::string publickey;

};


#endif // !CLIENTAUTHMESSAGE_H
