#pragma once

#include <string>
#include <memory>
#include <string_view>

#include "enums/MediaType.h"
#include "enums/MediaState.h"

namespace Media::Common {

    using MediaState = Media::Common::Enums::MediaState;
    using MediaType = Media::Common::Enums::MediaType;

    class MediaElement {
    public:
        MediaElement(MediaType mediaType, std::string_view id, std::shared_ptr<void> data = nullptr)
            : id_(id),
              mediaType_(mediaType),
              data_(data) {}
        
        virtual ~MediaElement() = default;

        std::string_view getId() const { return id_; }
        
        MediaState getState() const { return state_; }
        void setState(MediaState state) { state_ = state; }

        MediaType getType() const { return mediaType_; }
        void setState(MediaType type) { mediaType_ = type; }

        void setData(std::shared_ptr<void> data) { data_ = std::move(data); }
        std::shared_ptr<void> getData() const { return data_; }

    protected:
        std::string id_ = "";
        MediaType mediaType_ = MediaType::Unknown;
        MediaState state_ = MediaState::Unknown;
        std::shared_ptr<void> data_ = nullptr;
    private:
        MediaElement(const MediaElement&) = delete;
        MediaElement& operator=(const MediaElement&) = delete;
        MediaElement(MediaElement&&) = delete;
        MediaElement& operator=(MediaElement&&) = delete;
    };

} // namespace Media::Common