#pragma once
#include <memory>

#include "../common/MediaControler.h"
#include "../common/enums/MediaState.h"
#include "../common/adapters/VideoAdapters.h"
#include "../common/adapters/IVideoAdapter.h"

namespace Media {

class VideoPlayer : public Common::MediaControler {
    public:
        using MediaControler = Common::MediaControler;
        using MediaState = Common::Enums::MediaState;
        using IVideoAdapter = Adapters::IVideoAdapter;
        using AdapterType = Common::Adapters::AdapterType;

        VideoPlayer();
        VideoPlayer(AdapterType adapterType, VideoElement mediaElement, int argc = 0, char *argv[] = nullptr);
        ~VideoPlayer();

        bool loadMedia(const VideoElement& mediaElement, AdapterType adapterType = AdapterType::GstAdapter);
        bool changeAdapter(AdapterType adapterType);
        void play() override;
        void pause() override;
        void stop() override;
        void seek(double position) override;
        double getCurrentPosition() const override;
        double getDuration() const override;
        uint getIndex() const override;
        
    private:
        std::unique_ptr<IVideoAdapter> adapter_ = nullptr;
    };
} // namespace Media