#include "logger.h"

#include <log4cplus/consoleappender.h>

namespace logger {

    logger::logger(const std::string &name)
        :logger_{log4cplus::Logger::getInstance(LOG4CPLUS_TEXT(name))}
    {
        log4cplus::SharedAppenderPtr consoleAppender(new log4cplus::ConsoleAppender(false, true));
        logger_.addAppender(consoleAppender);
    }

}
