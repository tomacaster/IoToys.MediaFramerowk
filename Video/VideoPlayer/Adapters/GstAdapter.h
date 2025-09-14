#pragma once

#include <memory>
#include <thread>

#include "../../../common/adapters/IVideoAdapter.h"

#include "GstAdapterContext.h"

#include "GstAdapterTypes.h"

namespace Media::Adapters {

    class GstAdapter : public IVideoAdapter {
        public:
            GstAdapter(int argc = 0, char *argv[]= NULL);
            ~GstAdapter();

            void init() override;
            bool isInitialized() const override;
            bool loadMedia(const VideoElement& mediaElement) override;
            bool isReady() const override;
            std::optional<std::string_view> hasError() const override;

            void play() override;
            void pause() override;
            void stop() override;
            void seek(double position) override;
            double getCurrentPosition() const override;
            double getDuration() const override;

        private:
            gboolean terminate = FALSE;
            GstAdapterContext context_;
            GstStateChangeReturn ret;
            Wrappers::GstMessagePtr message_;;
            Wrappers::GstBusPtr bus_;
            std::stop_token stop_token_;
            static void padAddedHandler(GstElement *src, GstPad *new_pad, const GstAdapterContext &data);
            void handleMessageThread(std::stop_token stop_token, GstAdapterContext *context);
    };
}