//This class contains policies for the scheduler as key-value pairs.
class SchedulerConfig {
private:
    //This attribute contains all the policies for the scheduler.
    std::map<std::string, std::string> values_;


public:
    //This constructor creates an empty SchedulerConfig.
    SchedulerConfig();

    //This constructor loads a SchedulerConfig from a file.
     SchedulerConfig(const std::filesystem::path & & path);

    //This method returns the amount of key-value pairs in the SchedulerConfig.
    size_t count();

    //This method adds policy with a specified key to the SchedulerConfig.
    void Set(const std::string & & key, const std::string & & value);

    //This method returns the policy associated with the specified key. If the SchedulerConfig does not contain such a policy, an empty string is returned.
    std::string & Get(const std::string & & key);

    //This method clears out the SchedulerConfig.
    void Clear();

    //This method serializes the SchedulerConfig and saves it to the specified file.
    bool Save(const std::filesystem::path & & path);

    //This method returns true when the SchedulerCOnfig contains a policy associated with the specified key, otherwise it returns false.
    bool Contains(const std::string & & key);

};