#pragma once

namespace Media::Common::Enums {

    enum class MediaType : int {
        Audio = 0,
        Video = 1,
        Image = 2,
        Document = 3,
        Unknown = -1
    };

} // namespace Media::Common::Enums