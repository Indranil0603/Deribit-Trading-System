#include <array>
#include <string>
#include <iostream>
#include <functional>
#include <SubscriptionList.h>


SubscriptionList::SubscriptionList() : count(0), channels() {};

// Add a channel to the subscription list
bool SubscriptionList::addChannel(const std::string& channel) {
    if (count >= MAX_CHANNELS) {
        std::cerr << "Subscription list is full!" << std::endl;
        return false;
    }
    channels[count++] = channel;
    return true;
}

// Remove a channel from the subscription list
bool SubscriptionList::removeChannel(const std::string& channel) {
    for (size_t i = 0; i < count; ++i) {
        if (channels[i] == channel) {
            // Shift elements to fill the gap
            for (size_t j = i; j < count - 1; ++j) {
                channels[j] = channels[j + 1];
            }
            --count;
            return true;
        }
    }
    std::cerr << "Channel not found in the subscription list!" << std::endl;
    return false;
}

// Iterate over all subscribed channels
void  SubscriptionList::forEachChannel(const std::function<void(const std::string&)>& callback) const {
    for (size_t i = 0; i < count; ++i) {
        callback(SubscriptionList::channels[i]);
    }
}

// Get the current number of subscriptions
size_t SubscriptionList::size() const {
    return count;
}

