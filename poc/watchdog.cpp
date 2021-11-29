//
// Created by topnax on 28.11.21.
//

#include "watchdog.h"

void Watchdog::guard() {
    std::unique_lock<std::mutex> lock(mutex);
    if (!guarding) {
        guarding = true;
        // start a new thread, that periodically checks whether the watchdog has been kicked
        thread = std::thread(&Watchdog::run, this);
    }
}

void Watchdog::stop() {
    std::unique_lock<std::mutex> lock(mutex);
    if (guarding) {
        guarding = false;
        // notify all
        bite_condition.notify_all();
        // can unlock the mutex now
        lock.unlock();
        // join the thread, it should have been terminated now
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void Watchdog::run() {
    std::unique_lock<std::mutex> lock(mutex);

    while (guarding) {
        if (bite_condition.wait_for(lock, timeout) == std::cv_status::timeout ) {
            if (last_kick_time + timeout >= std::chrono::system_clock::now()) {
                // the watchdog has been recently kicked, but the thread has been spuriously woken up
            } else {
                // exit watchdog loop
                guarding = false;
                // call bite callback
                bite();
            }
        }
    }
}

void Watchdog::kick() {
    std::unique_lock<std::mutex> lock(mutex);
    last_kick_time = std::chrono::system_clock::now();
    bite_condition.notify_all();
}

/**
 * A namespace for a global watchdog instance
 */
namespace ThreadWatchdog {
    void set_doggo(Watchdog *new_doggo) {
        doggo = new_doggo;
    }

    void kick() {
        if (doggo != nullptr) {
            doggo->kick();
        }
    }

    void stop() {
        if (doggo != nullptr) {
            doggo->stop();
        }
    }

    void remove() {
        doggo = nullptr;
    }
}
