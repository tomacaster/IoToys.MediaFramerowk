#pragma once

namespace Media::Common::Enums {

    enum class MediaState : int {
        Playing = 0,
        Paused = 1,
        Stopped = 2,
        Buffering = 3,
        Seeking = 4,
        Ended = 5,
        Error = -1,
        Unknown = -2
    };
} // namespace Media::Common::Enums