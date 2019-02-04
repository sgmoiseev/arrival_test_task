#pragma once

#include <memory>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

namespace common {

    struct event_base_deleter {
        void operator()(event_base *ptr) const noexcept {
            event_base_free(ptr);
        }
    };

    using event_base_ptr = std::unique_ptr<event_base, event_base_deleter>;


    struct evconnlistener_deleter {
        void operator()(evconnlistener *ptr) const noexcept {
            evconnlistener_free(ptr);
        }
    };

    using listener_ptr = std::unique_ptr<evconnlistener, evconnlistener_deleter>;


    struct bufferevent_deleter {
        void operator()(bufferevent *ptr) const noexcept {
            bufferevent_free(ptr);
        }
    };

    using bufferevent_ptr = std::unique_ptr<bufferevent, bufferevent_deleter>;

}
