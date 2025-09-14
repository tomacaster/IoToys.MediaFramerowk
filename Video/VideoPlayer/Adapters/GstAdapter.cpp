#include "GstAdapter.h"

#include <thread>

#include <gstreamer-1.0/gst/gst.h>

namespace Media::Adapters {

    void GstAdapter::padAddedHandler (GstElement *src, GstPad *new_pad, const GstAdapterContext &data) {
    GstCaps *caps = gst_pad_get_current_caps(new_pad);
    GstStructure *str = gst_caps_get_structure(caps, 0);
    const gchar *name = gst_structure_get_name(str);

    GstPad *target_pad = nullptr;
    if (g_str_has_prefix(name, "audio/")) {
        target_pad = gst_element_get_static_pad(data.audioConvert.get(), "sink");
    } else if (g_str_has_prefix(name, "video/")) {
        target_pad = gst_element_get_static_pad(data.videoConvert.get(), "sink");
    }

    if (target_pad) {
        if (!gst_pad_is_linked(target_pad)) {
            if (gst_pad_link(new_pad, target_pad) != GST_PAD_LINK_OK) {
                g_printerr("Pad link failed for %s\n", name);
            }
        }
        gst_object_unref(target_pad);
    }
    gst_caps_unref(caps);
    }

    GstAdapter::GstAdapter(int argc, char *argv[])
    {
        if (!gst_is_initialized()) {
            g_print("Initializing now GStreamer\n");

            gst_init(&argc, &argv);
        } else {    
            g_print("GStreamer is already initialized.\n");
        }       

        if (stop_token_.stop_possible()) {
            stop_token_ = std::stop_token();
        } else {
            g_printerr("Stop token is not possible to use.\n");
        }
    }

    void GstAdapter::init()
    {
        context_.source = Wrappers::GstElementPtr(gst_element_factory_make("uridecodebin", "source"));
        context_.audioConvert = Wrappers::GstElementPtr(gst_element_factory_make ("audioconvert", "convert"));
        context_.audioResample = Wrappers::GstElementPtr(gst_element_factory_make ("audioresample", "resample"));
        context_.audioSink = Wrappers::GstElementPtr(gst_element_factory_make ("autoaudiosink", "sink"));

        context_.videoConvert = Wrappers::GstElementPtr(gst_element_factory_make("videoconvert", "video_convert"));
        context_.videoScale = Wrappers::GstElementPtr(gst_element_factory_make("videoscale",   "video_scale"));
        context_.videoSink = Wrappers::GstElementPtr(gst_element_factory_make("autovideosink","video_sink"));

        /* Create the empty pipeline */
        context_.pipeline = Wrappers::GstElementPtr(gst_pipeline_new ("gstPipeline"));

        if (!context_.pipeline || !context_.source || !context_.audioConvert || !context_.audioResample || !context_.audioSink
            || !context_.videoConvert || !context_.videoScale || !context_.videoSink) {
            g_printerr ("Not all elements could be created.\n");
            
            return;
        }

        /* Build the pipeline. Note that we are NOT linking the source at this
        * point. We will do it later. */
        gst_bin_add_many (GST_BIN (context_.pipeline.get()), context_.source.get(), context_.audioConvert.get(), context_.audioResample.get(), context_.audioSink.get(), 
            context_.videoConvert.get(), context_.videoScale.get(), context_.videoSink.get(), nullptr);

        auto audioErr = gst_element_link_many(context_.audioConvert.get(), context_.audioResample.get(), context_.audioSink.get(), nullptr);
        auto videoErr = gst_element_link_many(context_.videoConvert.get(), context_.videoScale.get(), context_.videoSink.get(), nullptr);

        if (!audioErr || !videoErr) {
            g_printerr ("Elements could not be linked.\n");
            context_.pipeline.reset();

            return; 
        }
    }

    GstAdapter::~GstAdapter() {}

    bool GstAdapter::isInitialized() const
    {
        return (gst_is_initialized());
    }

    bool GstAdapter::loadMedia(const VideoElement &mediaElement)
    {
        if (!isInitialized()) {
            g_printerr("GStreamer is not initialized.\n");
            return false;
        }
        
        if (!context_.pipeline || !context_.source || !context_.audioConvert || !context_.audioResample || !context_.audioSink
            || !context_.videoConvert || !context_.videoScale || !context_.videoSink) {
            g_printerr("Pipeline or elements are not created.\n");
            return false;
        }
        /* Set the URI to play */
        g_object_set (context_.source.get(), "uri", mediaElement.getUri().data(), NULL);

        /* Connect to the pad-added signal */
        g_signal_connect (context_.source.get(), "pad-added", G_CALLBACK (&GstAdapter::padAddedHandler), &context_);

        return true;
    }

    bool GstAdapter::isReady() const
    {
        return false;
    }

    std::optional<std::string_view> GstAdapter::hasError() const
    {
        return std::nullopt;
    }

    void GstAdapter::play() {
        if (!context_.pipeline) {
            g_printerr("Pipeline is not created.\n");
            return;
        }

        ret = gst_element_set_state (context_.pipeline.get(), GST_STATE_PLAYING);

        if (ret == GST_STATE_CHANGE_FAILURE) {
            g_printerr ("Unable to set the pipeline to the playing state.\n");
            context_.pipeline.reset();

            return;
        }

        std::jthread message_thread(&GstAdapter::handleMessageThread, this, stop_token_, &context_);

    }

    void GstAdapter::pause() { /* przykładowo: gst_element_set_state(pipeline, GST_STATE_PAUSED); */ }
    void GstAdapter::stop() { /* przykładowo: gst_element_set_state(pipeline, GST_STATE_NULL); */ }
    void GstAdapter::seek(double position) { /* ... */ }
    double GstAdapter::getCurrentPosition() const { return 0.0; }
    double GstAdapter::getDuration() const { return 0.0; }

    void GstAdapter::handleMessageThread(std::stop_token stop_token, GstAdapterContext *context)
    {
        bus_ = Wrappers::GstBusPtr(gst_element_get_bus(context_.pipeline.get()));
        do {
            message_ = Wrappers::GstMessagePtr(gst_bus_timed_pop_filtered (bus_.get(), GST_CLOCK_TIME_NONE,
                (GstMessageType)(GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS)));

            /* Parse message */
            if (message_.get() != NULL) {
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE (message_.get())) {
                case GST_MESSAGE_ERROR:
                    gst_message_parse_error (message_.get(), &err, &debug_info);
                    g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (message_.get()->src), err->message);
                    g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
                    g_clear_error (&err);
                    g_free (debug_info);
                    terminate = TRUE;

                    break;
                case GST_MESSAGE_EOS:
                    g_print ("End-Of-Stream reached.\n");
                    terminate = TRUE;

                    break;
                case GST_MESSAGE_STATE_CHANGED:
                    /* We are only interested in state-changed messages from the pipeline */
                    if (GST_MESSAGE_SRC (message_.get()) == GST_OBJECT (context_.pipeline.get())) {
                        GstState old_state, new_state, pending_state;
                        gst_message_parse_state_changed (message_.get(), &old_state, &new_state, &pending_state);
                        g_print ("Pipeline state changed from %s to %s:\n",
                            gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
                    }

                    break;
                default:
                    /* We should not reach here */
                    g_printerr ("Unexpected message received.\n");

                    break;
            }

            if (message_) {
                message_.reset();
            }
        }
        } while (!terminate || !stop_token.stop_requested());

        /* Free resources */
        bus_.reset();
        gst_element_set_state (context_.pipeline.get(), GST_STATE_NULL);
        context_.pipeline.reset();
    }
}
