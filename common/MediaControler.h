#pragma once

namespace Media::Common {
    
    // MediaControler class definition
    class MediaControler {
    public:
        MediaControler() = default;
        virtual ~MediaControler() = default;    
        // Control methods
        virtual void play() = 0;
        virtual void pause() = 0;
        virtual void stop() = 0;
        virtual void seek(double position) = 0;
        virtual double getCurrentPosition() const = 0;
        virtual double getDuration() const = 0;
        virtual unsigned int getIndex() const { return 0; } // Default implementation, can be overridden;
    protected:
        // Protected members can be added here if needed
    };
} // namespace Media::Common::MediaControler