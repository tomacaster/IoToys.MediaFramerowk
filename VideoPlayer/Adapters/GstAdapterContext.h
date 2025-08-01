#pragma once

#include <string>
#include <optional>

#include <gstreamer-1.0/gst/gst.h>

#include "GstAdapterTypes.h"

namespace Media::Adapters {

    class GstAdapterContext {
        public:
            Wrappers::GstElementPtr pipeline;
            Wrappers::GstElementPtr source;
            Wrappers::GstElementPtr convert;
            Wrappers::GstElementPtr resample;
            Wrappers::GstElementPtr sink;
    };

} // namespace Media::Adapters