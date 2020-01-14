class Timer {
public:
    //Blockiert den Aufrufer fùr eine Anzahl an Sekunden
    //
    //s: Anzahl der zu wartenden Sekunden
    void wait(int s);

    //Wartet s Sekunden und fùhrt dann die Funktion func aus.
    //Der Aufrufer wird nicht blockiert
    //
    //s: Anzahl der zu wartenden Sekunden
    //func: Die auszufùhrende Funktion
    void wait(int s, const std::function<void()>& func,  );
};