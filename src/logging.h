/**
 * Source code for logging macro from:
 * https://stackoverflow.com/questions/19415845/a-better-log-macro-using-template-metaprogramming
 *
 * Created by Martin on 23.11.2021.
 * me@martinforejt.cz
 *
 * Author: https://stackoverflow.com/questions/19415845/a-better-log-macro-using-template-metaprogramming
 */
#ifndef PPR_LOGGING_H
#define PPR_LOGGING_H

#include <iostream>

// Define debug mode
#define DEBUG

#ifndef NOINLINE_ATTRIBUTE
#ifdef __ICC
#define NOINLINE_ATTRIBUTE __attribute__(( noinline ))
#else
#define NOINLINE_ATTRIBUTE
#endif // __ICC
#endif // NOINLINE_ATTRIBUTE

struct None {
};

template<typename List>
struct LogData {
    List list;
};

template<typename Begin, typename Value>
constexpr LogData<std::pair<Begin &&, Value &&>> operator<<(LogData<Begin> &&begin,
                                                            Value &&value) noexcept {
    return {{std::forward<Begin>(begin.list), std::forward<Value>(value)}};
}

template<typename Begin, size_t n>
constexpr LogData<std::pair<Begin &&, const char *>> operator<<(LogData<Begin> &&begin,
                                                                const char (&value)[n]) noexcept {
    return {{std::forward<Begin>(begin.list), value}};
}

typedef std::ostream &(*PfnManipulator)(std::ostream &);

template<typename Begin>
constexpr LogData<std::pair<Begin &&, PfnManipulator>> operator<<(LogData<Begin> &&begin,
                                                                  PfnManipulator value) noexcept {
    return {{std::forward<Begin>(begin.list), value}};
}

template<typename Begin, typename Last>
void output(std::ostream &os, std::pair<Begin, Last> &&data) {
    output(os, std::move(data.first));
    os << data.second;
}

inline void output(std::ostream &os, None) {}

template<typename List>
void Log(const char *file, int line,
         LogData<List> &&data) NOINLINE_ATTRIBUTE {
    std::cout << std::dec;
    if (line != -1) {
        std::cout << file << ":" << line << ": ";
    }
    output(std::cout, std::move(data.list));
    std::cout << std::endl;
};

#define FILENAME (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define LOG(x) (Log("",-1,LogData<None>() << x))

#ifdef DEBUG
#define LOG_D(x) (Log(FILENAME,__LINE__,LogData<None>() << x))
#else
#define LOG_D(str) do { } while ( false )
#endif

#endif /* PPR_LOGGING_H */
