#pragma once
#if 0
#include <worker/docker/Container.h>

namespace balancedbanana::worker {
class Worker {
public:
	Container getContainerbyTaskID(unsigned long tid);

	//Ermittelt HW Details
	//Erschafft min einen Verbindungsempf�nger (MsgProcessor + Communicator)
	//initialisiert Aufgabenverarbeitung
	//liest hinterlegte Daten ein (vorheriger Server, unterbrochene Aufgaben)
	//setzt unterbrochene Aufgaben fort
	//meldet sich bei Server an, falls m�glich, ansonsten: wartet auf Ansto� eines Servers
	void initialize();

	//Ermittelt CPU Kern Anzahl, RAM, Betriebssystem
	void getHardwareDetails();

	//Liest Eintr�ge aus einer Datei und interpretiert diese als Aufgaben (DockerImmages oder so)
	void readInTaskList(const std::string& pathToTaskList);

	//Speichert den Snapshot auf dem File Server, damit ein Benutzer darauf zugriff bekommt
	//
	//location: wo auf dem FileServer soll der Snapshot gespeichert werden.
	void moveSnapshotToFileServer(const Snapshot& snapshot, const std::string& location);

	//Speichert einen Snapshot aller laufenden Aufgaben an der angegebenen Stelle im FileServer oder nur auf dem Worker, falls location keine g�ltige Stelle auf dem File Server ist.
	void backup(const std::string& location);


private:

};
}
#else
#pragma once

#include <memory>

#include <communication/Task.h>
#include <communication/Communicator.h>
#include <configfiles/ApplicationConfig.h>
#include "WorkerMP.h"


namespace balancedbanana
{
namespace worker
{

class Worker
{
public:

    Worker();

    void connectWithServer(const std::string& serverIpAdress, short serverPort);
    void authenticateWithServer();
    void processCommandLineArguments(int argc, const char* const * argv);

private:
    balancedbanana::configfiles::ApplicationConfig config;
    std::shared_ptr<balancedbanana::communication::Communicator> communicator;
    std::shared_ptr<WorkerMP> workerMP;
    std::shared_ptr<balancedbanana::communication::Task> task;
};

} // namespace worker

} // namespace balancedbanana
#endif