#pragma once

#include <string>

namespace Media::Common {

    class MediaDescription {
        public:
            double durationMs; // in miliseconds
            int sizeBytes; // in bytes

            std::string title;
            std::string description;
            std::string author;
            std::string copyright;
            std::string license;
            std::string language;
            std::string genre;
            std::string releaseDate; // ISO 8601 format
            std::string rating; // e.g., "PG-13", "R"
            std::string tags; // Comma-separated tags
            std::string thumbnailUri; // URI to the thumbnail image
            std::string sourceUri; // URI to the original media source
            

    };
} // namespace Media::Common