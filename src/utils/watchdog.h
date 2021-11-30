//
// Created by topnax on 28.11.21.
//

#pragma once

#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <utility>

const uint32_t WATCHDOG_DEFAULT_BITE_TIMEOUT = 8;

/**
 * A class representing a thread watchdog, that, once asked to guard, bites, whenever it has not been kicked
 * for a given time.
 */
class Watchdog {
public:
    /**
     * Creates an instance of a watchdog, passing the bite timeout in seconds and bite callback that is moved.
     *
     * @param timeout       timeout before the dog bites (in seconds)
     * @param bite_callback callback to be invoked when the dog decides to bite
     */
    Watchdog(uint32_t timeout, std::function<void()> bite_callback) : timeout(timeout * 1000),
                                                                      bite(std::move(bite_callback)) {};

    // overload operators
    Watchdog(Watchdog &w) = delete;

    void operator=(const Watchdog &) = delete;

    /**
     * Ask the dog to guard. Requires kicking to prevent from biting.
     */
    void guard();

    /**
     * Ask the dog to stop guarding
     */
    void stop();

    /**
     * Kick the watchdog so that it does not bite
     */
    void kick();

private:
    /**
     * Private method that periodically sleeps for the given duration (timeout), and calls the bite callback if not kicked
     * recently.
     */
    void run();

    /**
     * Callback to be triggered once a watchdog bites!!!
     */
    std::function<void()> bite;

    /**
     * A timeout before a watchdog decides to bite!!!
     */
    std::chrono::milliseconds timeout;

    /**
     * Flag indicating whether the watchdog is guarding.
     */
    bool guarding = false;

    /**
     * The last time the watchdog has been kicked. (used to catch spurious wake ups)
     */
    std::chrono::time_point<std::chrono::system_clock> last_kick_time;

    /**
     * The thread the watchdog guards in (loops and checks whether it has been kicked).
     */
    std::thread thread;

    /**
     * Tool for synchronization access from multiple threads and for the condition variable
     */
    std::mutex mutex;

    /**
     * A condition variable in order to be able to implement a timeout.
     */
    std::condition_variable bite_condition;
};

namespace ThreadWatchdog {
    namespace {
        Watchdog *doggo = nullptr;
    }

    /**
     * Sets the instance of the global doggo
     *
     * @param new_doggo doggo to be set
     */
    void set_doggo(Watchdog *new_doggo);

    /**
     * Kicks the global doggo, if it exists.
     */
    void kick();

    /**
     * Stops the global doggo from guarding
     */
    void stop();

    /**
     * Removes the global doggo
     */
    void remove();
}
