
#include "components/mptf_core/common/include/Logger.h"

#include "Video/VideoElement/VideoElement.h"
#include "Video/VideoPlayer/VideoPlayer.h"
// #include "VideoPlayer/Adapter/GstAdapter.h"
// #include "VideoPlayer/Adapter/IVideoAdapter.h"
// #include <gst/gst.h>
// #include <iostream>

int main(int argc, char *argv[]) {
    Media::VideoElement videoElement("https://gstreamer.freedesktop.org/data/media/sintel_trailer-480p.webm");
    Media::VideoPlayer videoPlayer;
    Logger::InitLogger("");
        auto logger = Logger::GetClassLogger("Main");
    logger->info("Starting video playback...");
    videoPlayer.loadMedia(videoElement, Media::Common::Adapters::AdapterType::GstAdapter);
    videoPlayer.play();
    // gst_init(&argc, &argv);
    // std::cout << "GStreamer initialized successfully!" << std::endl;

    // Media::Common::MediaElement element("id1", Media::Common::ElementState::Initialized);

    // GstElement *pipeline = gst_parse_launch("fakesrc ! fakesink", nullptr);
    // if (!pipeline) {
    //     std::cerr << "Failed to create pipeline." << std::endl;
    //     return 1;
    // }

    // gst_element_set_state(pipeline, GST_STATE_PLAYING);
    // std::cout << "Pipeline is running..." << std::endl;
    // gst_element_set_state(pipeline, GST_STATE_NULL);
    // gst_object_unref(pipeline);
    // std::cout << "Pipeline stopped and cleaned up." << std::endl;
    return 0;
}
