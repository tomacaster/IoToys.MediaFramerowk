#include "VideoElement.h"
#include <iostream> // do ewentualnego debugowania

namespace Media {
    using MediaType = Media::Common::MediaType;
    using MediaState = Media::Common::Enums::MediaState;

    // VideoElement::VideoElement(std::string_view id, std::shared_ptr<void> data) : 
    //     MediaElement(MediaType::Video, id, std::move(data))
    // {

    // }

    VideoElement::VideoElement(std::string_view uri): 
        MediaElement(MediaType::Video, "123", nullptr),
        uri_(uri)
    {

    }

} // namespace Media