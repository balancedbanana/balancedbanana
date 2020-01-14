//Priorizes Task
class PriorityQueue : public Queue {
public:
    virtual addTask(const Job & job);

    virtual unsigned long getPos(unsigned long id);

    virtual Job getJob(const uint32_t & ram, const uint32_t & cores);

    virtual  update();


private:
    Timer * ;

};