
#include "bbdsrc.h"
#include "bbcsrc.h"

void WorkerMP::processMsgY(const std::shared_ptr<MsgY> & msg) {
}

 Worker::processSnapshot(const Message & msg) {
}

Container Worker::getContainerbyTaskID(unsigned long tid) {
}

//Ermittelt HW Details
//Erschafft min einen Verbindungsempfï¿½nger (MsgProcessor + Communicator)
//initialisiert Aufgabenverarbeitung
//liest hinterlegte Daten ein (vorheriger Server, unterbrochene Aufgaben)
//setzt unterbrochene Aufgaben fort
//meldet sich bei Server an, falls mï¿½glich, ansonsten: wartet auf Anstoï¿½ eines Servers
void Worker::initialize() {
}

//Ermittelt CPU Kern Anzahl, RAM, Betriebssystem
void Worker::getHardwareDetails() {
}

//Liest Eintrï¿½ge aus einer Datei und interpretiert diese als Aufgaben (DockerImmages oder so)
void Worker::readInTaskList(const std::string & pathToTaskList) {
}

//Speichert den Snapshot auf dem File Server, damit ein Benutzer darauf zugriff bekommt
//
//location: wo auf dem FileServer soll der Snapshot gespeichert werden.
void Worker::moveSnapshotToFileServer(const Snapshot & snapshot, const std::string & location) {
}

//Speichert einen Snapshot aller laufenden Aufgaben an der angegebenen Stelle im FileServer oder nur auf dem Worker, falls location keine gültige Stelle auf dem File Server ist.
void Worker::backup(const std::string & location) {
}

string Container::Tail(int lines) {
}

void Container::Stop() {
}

Snapshot Container::Snapshot(bool stop) {
}

 Container::Resume(const Snapshot & snap) {
}

Container Docker::Start(int userid, const Task & task) {
}

Snapshot::Snapshot(const std::string & id) {
}

