#pragma once


//Nachticht mit allen Hardware Angaben (CPU RAM OS)
class HardwareDetailMessage : public Message {
public:
	void process(const MessageProcessor* [balancedbanana::communication] & msgProcessor);


private:
	//Anzahl der CPU Kerne
	int coreCount;

	//Gr��e des verf�gbaren Arbeitsspeichers
	int ramSize;

	//Irgendeine Information, die das Betriebssystem identifiziert
	std::string osIdentifier;


public:
	HardwareDetailMessage(int coreCount, int ramSize, const std::string& osIdentifier);

};