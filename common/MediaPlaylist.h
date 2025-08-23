#pragma once

namespace Media::Common {

    class MediaPlaylist {
    public:
        MediaPlaylist() = default;
        virtual ~MediaPlaylist() = default;

        // Add a media element to the playlist
        virtual void addMediaElement(const std::string& mediaId) = 0;

        // Remove a media element from the playlist
        virtual void removeMediaElement(const std::string& mediaId) = 0;

        // Get the current media element
        virtual std::string getCurrentMediaElement() const = 0;

        // Play the current media element
        virtual void play() = 0;

        // Pause the current media element
        virtual void pause() = 0;

        // Stop playback of the current media element
        virtual void stop() = 0;

    private:
        MediaPlaylist(const MediaPlaylist&) = delete;
        MediaPlaylist& operator=(const MediaPlaylist&) = delete;
    };

} // namespace Media::Common