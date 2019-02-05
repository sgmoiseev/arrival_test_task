#pragma once

#include <sstream>
#include <log4cplus/logger.h>

namespace logger {

    class logger {
    public:
        explicit logger(const std::string &name);

    public:
        template<typename ...args_t>
        void info(args_t &&...args)
        {
            log(log4cplus::INFO_LOG_LEVEL, std::forward<args_t>(args)...);
        }

        template<typename ...args_t>
        void error(args_t &&...args)
        {
            log(log4cplus::ERROR_LOG_LEVEL, std::forward<args_t>(args)...);
        }

    private:
        template<typename arg_t, typename ...args_t>
        void args_to_stream(std::stringstream &stream, arg_t &&arg, args_t &&...args)
        {
            stream << std::forward<arg_t>(arg);
            args_to_stream(stream, std::forward<args_t>(args)...);
        }

        template<typename arg_t>
        void args_to_stream(std::stringstream &stream, arg_t &&arg)
        {
            stream << std::forward<arg_t>(arg) << ".";
        }

        template<typename ...args_t>
        void log(log4cplus::LogLevel level, args_t &&...args)
        {
            std::stringstream stream;
            args_to_stream(stream, std::forward<args_t>(args)...);
            logger_.log(level, stream.str());
        }

    private:
        log4cplus::Logger logger_;
    };

}
