class Queue {
public:
    virtual  addTask(const Job & job) = 0;

    virtual uint64_t getPos(const uint64_t & id);

    virtual Job getJob(const uint64_t & id) = 0;

    virtual  update() = 0;

private:
    std::shared_ptr<Scheduler> scheduler;

};