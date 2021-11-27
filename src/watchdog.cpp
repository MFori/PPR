/**
 * Created by Martin on 25.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#include "watchdog.h"

#include <utility>

Watchdog *Watchdog::instance = nullptr;

void Watchdog::create_instance() {
    if (instance == nullptr) {
        instance = new Watchdog();
    }
}

void Watchdog::destroy() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}

void Watchdog::start(unsigned int timeout_seconds, std::function<void()> bite_callback) {
    create_instance();
    Watchdog::instance->start_(timeout_seconds, std::move(bite_callback));
}

void Watchdog::stop() {
    create_instance();
    Watchdog::instance->stop_();
}

void Watchdog::kick() {
    create_instance();
    Watchdog::instance->kick_();
}

void Watchdog::start_(unsigned int timeout_seconds, std::function<void()> bite_callback) {
    std::unique_lock<std::mutex> lock(mutex);

    if (!running) {
        running = true;
        timeout = std::chrono::seconds(timeout_seconds);
        bite = std::move(bite_callback);

        // start watchdog thread
        thread = std::thread(&Watchdog::run, this);
    }
}

void Watchdog::stop_() {
    std::unique_lock<std::mutex> lock(mutex);
    if (running) {
        running = false;
        stop_condition.notify_all();
        lock.unlock();
        if (thread.joinable()) {
            thread.join();
        }
        Watchdog::destroy();
    }
}

void Watchdog::kick_() {
    std::unique_lock<std::mutex> lock(mutex);
    stop_condition.notify_all();
}

void Watchdog::run() {
    std::unique_lock<std::mutex> lock(mutex);

    while (running) {
        if (stop_condition.wait_for(lock, timeout) == std::cv_status::timeout) {
            // exit watchdog loop
            running = false;
            // call bite callback
            bite();
        }
    }
}