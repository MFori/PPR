/**
 * Created by Martin on 24.11.2021.
 * me@martinforejt.cz
 *
 * Author: Martin Forejt
 */
#ifndef PPR_WATCHDOG_H
#define PPR_WATCHDOG_H

#include <functional>
#include <chrono>
#include <mutex>

class Watchdog {
public:
    Watchdog(Watchdog &other) = delete;

    void operator=(const Watchdog &) = delete;

    static void start(unsigned int timeout_seconds, std::function<void()> bite_callback);

    static void stop();

    static void kick();

    static void destroy();

private:
    Watchdog() {};

    static void create_instance();

    void start_(unsigned int timeout_seconds, std::function<void()> bite_callback);

    void stop_();

    void kick_();

    void run();

    static Watchdog *instance;
    std::function<void()> bite;
    std::chrono::seconds timeout;
    bool running = false;
    std::mutex mutex;
    std::condition_variable stop_condition;
    std::thread thread;
};

#endif /* PPR_WATCHDOG_H */
