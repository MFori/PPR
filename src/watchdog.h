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

/**
 * Watchdog
 *
 * Watchdog is started via Watchdog::start passing timeout (s) and bite callback,
 * which is called when timeout ticks out without kicking watchdog via Watchdog::kick
 */
class Watchdog {
public:
    Watchdog(Watchdog &other) = delete;

    void operator=(const Watchdog &) = delete;

    /**
     * Start watchdog
     * @param timeout_seconds watchdog timeout in seconds
     * @param bite_callback bite callback function
     */
    static void start(unsigned int timeout_seconds, std::function<void()> bite_callback);

    /**
     * Stop watchdog
     */
    static void stop();

    /**
     * Kick watchdog, to prevent bite
     */
    static void kick();

private:
    // private constructor
    Watchdog() {};

    /**
     * Create new instance of watchdog if not exists
     */
    static void create_instance();

    /**
     * Destroy watchdog instance if exists
     */
    static void destroy();

    /**
     * Start watchdog
     * @param timeout_seconds timeout_seconds watchdog timeout in seconds
     * @param bite_callback bite_callback bite callback function
     */
    void start_(unsigned int timeout_seconds, std::function<void()> bite_callback);

    /**
     * Stop watchdog
     */
    void stop_();

    /**
     * Kick watchdog
     */
    void kick_();

    /**
     * Run watchdog loop, may run in watchdog thread
     */
    void run();

    /**
     * Singleton instance
     */
    static Watchdog *instance;
    /**
     * Bite callback
     */
    std::function<void()> bite;
    /**
     * Timeout in seconds
     */
    std::chrono::seconds timeout;
    /**
     * Is watchdog running?
     */
    bool running = false;
    /**
     * Mutex for condition variable
     */
    std::mutex mutex;
    /**
     * Condition variable for stop
     */
    std::condition_variable stop_condition;
    /**
     * Watchdog thread
     */
    std::thread thread;
};

#endif /* PPR_WATCHDOG_H */
