#pragma once

#include "../../common/MediaElement.h"
#include "../../common/enums/MediaType.h"
#include "../../common/MediaDescription.h"

namespace Media {
    using MediaElement = Media::Common::MediaElement;
    using MediaType = Media::Common::MediaType;
    using MediaDescription = Media::Common::MediaDescription;

    class VideoElement : public MediaElement {
        public:
           // VideoElement(std::string_view id, std::shared_ptr<void> data = nullptr);
            VideoElement(std::string_view uri);
            ~VideoElement() = default;

            std::string_view getUri() const { return std::string_view(uri_); }
            void setUri(std::string_view uri) { uri_ = uri; }
            const MediaDescription& getDescription() const { return description_; }
            void setDescription(const MediaDescription& desc) { description_ = desc; }

        private:
            MediaDescription description_;
            std::string uri_;
    };
} // namespace Media::VideoElement