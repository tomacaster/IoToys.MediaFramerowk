#include "VideoPlayer.h"

#include "Adapters/GstAdapter.h"

using namespace Media::Adapters;

namespace Media {
    void VideoPlayer::play()
    {
        if (adapter_) {
            adapter_->play();
        }
    }

    void VideoPlayer::pause() 
    {
        if (adapter_) {
            adapter_->pause();
        }
    }

    void VideoPlayer::stop() 
    {
        if (adapter_) {
            adapter_->stop();
        }
    }

    void VideoPlayer::seek(double position) 
    {
        if (adapter_) {
            adapter_->seek(position);
        }
    }

    double VideoPlayer::getCurrentPosition() const 
    {
        if (adapter_) {
            return adapter_->getCurrentPosition();
        }

        return 0.0;
    }

    double VideoPlayer::getDuration() const 
    {
        if (adapter_) {
            return adapter_->getDuration();
        }

        return 0.0; 
    }
    uint VideoPlayer::getIndex() const
    {
        return uint();
    }
    VideoPlayer::~VideoPlayer()
    {
        
    }
    VideoPlayer::VideoPlayer()
    {
    }
    VideoPlayer::VideoPlayer(AdapterType adapterType, VideoElement mediaElement, int argc, char *argv[])
    {
    }

    bool VideoPlayer::loadMedia(const VideoElement &mediaElement, AdapterType adapterType)
    {
        switch (adapterType)
        {
            case AdapterType::GstAdapter:
                    adapter_ = std::make_unique<GstAdapter>();
                    adapter_->init();
                    
                    return adapter_->loadMedia(mediaElement);
                break;
            
            default:
                break;
        }
        
        return false;
    }
    bool VideoPlayer::changeAdapter(AdapterType adapterType)
    {
        return false;
    }
}