class SchedulerClientMP : public MessageProcessor {
  public:
    virtual void processMsgX(const std::shared_ptr<MsgX> & msg);

    virtual void processMsgY(const std::shared_ptr<MsgY> & msg);

    virtual void processTaskMessage(const TaskMessage & msg);

};