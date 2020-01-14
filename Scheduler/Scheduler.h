class Scheduler {
public:
    void onTaskMessage(const Message & message);

    Worker getWorkerByTaskID(unsigned long tid);

    update();


private:
    std::vector<std::shared_ptr<Worker>> worker;
    


public:
    //L�dt existierende oder erstellt neue Datenbank
    //Liest Liste aller Arbeiter ein, falls vorhanden
    //Erschafft Verbindungsempf�nger f�r Clients und Arbeiter (min 2)
    //Liest Aufgabenliste aus DB aus
    //Liest vorhandene oder erstellt fehlende Konfigurationsdatei
    //Startet HTTP und SMTP Server
    //Fr�gt Status der Arbeiter ab (sollen fertige Aufgaben zur�ckmelden)
    //Startet Bearbeitung der Warteschlange (startet den Timer)
    //Sendet Nachricht an alle Arbeiter (ich bin hier nachricht)
    void initialize();


  private:
    SmtpServer * smtpServer;

    HttpServer * httpServer;

    Queue * queue;

    std::vector<std::shared_ptr<User>> users;

    <Job *> jobs;

    EventDispatcher * dispatcher;

};