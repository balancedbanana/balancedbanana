#pragma once


class Worker {
public:
	processSnapshot(const Message& msg);

	Container getContainerbyTaskID(unsigned long tid);

	//Ermittelt HW Details
	//Erschafft min einen Verbindungsempfï¿½nger (MsgProcessor + Communicator)
	//initialisiert Aufgabenverarbeitung
	//liest hinterlegte Daten ein (vorheriger Server, unterbrochene Aufgaben)
	//setzt unterbrochene Aufgaben fort
	//meldet sich bei Server an, falls mï¿½glich, ansonsten: wartet auf Anstoï¿½ eines Servers
	void initialize();

	//Ermittelt CPU Kern Anzahl, RAM, Betriebssystem
	void getHardwareDetails();

	//Liest Eintrï¿½ge aus einer Datei und interpretiert diese als Aufgaben (DockerImmages oder so)
	void readInTaskList(const std::string& pathToTaskList);

	//Speichert den Snapshot auf dem File Server, damit ein Benutzer darauf zugriff bekommt
	//
	//location: wo auf dem FileServer soll der Snapshot gespeichert werden.
	void moveSnapshotToFileServer(const Snapshot& snapshot, const std::string& location);

	//Speichert einen Snapshot aller laufenden Aufgaben an der angegebenen Stelle im FileServer oder nur auf dem Worker, falls location keine gültige Stelle auf dem File Server ist.
	void backup(const std::string& location);


private:
	Container* container;

};