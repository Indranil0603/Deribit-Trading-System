#ifndef SUBSCRIPTION_LIST_H
#define SUBSCRIPTION_LIST_H

#include <array>
#include <string>
#include <functional>

class SubscriptionList {
    private:
        static const size_t MAX_CHANNELS = 100; // Fixed size
        std::array<std::string, MAX_CHANNELS> channels; // Fixed-size array
        size_t count;
    
    public:
        SubscriptionList();
        bool addChannel(const std::string &);
        bool removeChannel(const std::string &);
        size_t size() const;
        void forEachChannel(const std::function<void(const std::string&)>& callback) const;

};


#endif /* SUBSCRIPTION_LIST_H */
