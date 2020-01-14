#ifndef _BBDSRC_H
#define _BBDSRC_H


#include "bb.h"
#include <string>


class Message;
class Communicator;
class Authenticator;
class Task;

class WorkerMP : public MessageProcessor {
  public:
    virtual void processMsgY(const std::shared_ptr<MsgY> & msg);

};
class Worker {
  public:
     processSnapshot(const Message & msg);

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
    void readInTaskList(const std::string & pathToTaskList);

    //Speichert den Snapshot auf dem File Server, damit ein Benutzer darauf zugriff bekommt
    //
    //location: wo auf dem FileServer soll der Snapshot gespeichert werden.
    void moveSnapshotToFileServer(const Snapshot & snapshot, const std::string & location);

    //Speichert einen Snapshot aller laufenden Aufgaben an der angegebenen Stelle im FileServer oder nur auf dem Worker, falls location keine g�ltige Stelle auf dem File Server ist.
    void backup(const std::string & location);


  private:
    Container * container;

};
class Container {
  public:
    string Tail(int lines);

    void Stop();

    Snapshot Snapshot(bool stop = false);

     Resume(const Snapshot & snap);


  private:
    string id;

    <Worker *> ;

};
class Docker {
  public:
    Container Start(int userid, const Task & task);

};
class Snapshot {
  private:
    string id;


  public:
    inline const string get_id() const;

    Snapshot(const std::string & id);

};
inline const string Snapshot::get_id() const {
  return id;
}

#endif
