#pragma once

#include <gst/gst.h>
#include <memory>

namespace Media::Adapters::Wrappers {

    struct GstMessageDeleter {
        void operator()(GstMessage* msg) const {
            if (msg) gst_message_unref(msg);
        }
    };

    struct GstObjectDeleter {
        template <typename T>
        void operator()(T* obj) const {
            if (obj) {
                gst_object_unref(GST_OBJECT(obj));
            }
        }
    };

    template <typename T>
    using GstMessageHandler = std::unique_ptr<T, GstMessageDeleter>;
    
    template <typename T>
    using GstObjectHandler = std::unique_ptr<T, GstObjectDeleter>;

    using GstMessagePtr = GstMessageHandler<GstMessage>;
    using GstBusPtr = GstObjectHandler<GstBus>;
    using GstElementPtr = GstObjectHandler<GstElement>;
}