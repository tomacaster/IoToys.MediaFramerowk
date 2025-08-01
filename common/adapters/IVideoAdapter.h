#pragma once

#include <optional>
#include <string_view>

#include "MediaControler.h"
#include "../VideoElement/VideoElement.h"

namespace Media::Adapters {
    using VideoElement = Media::VideoElement;

    class IVideoAdapter : public Media::Common::MediaControler {
        public:
            virtual ~IVideoAdapter() = default;

            virtual void init() = 0;
            virtual bool isInitialized() const = 0;
            virtual bool loadMedia(const VideoElement& mediaElement) = 0;
            virtual bool isReady() const = 0;
            virtual std::optional<std::string_view> hasError() const = 0;
    };
}