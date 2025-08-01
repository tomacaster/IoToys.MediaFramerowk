#include "GstAdapter.h"

#include <thread>

#include <gstreamer-1.0/gst/gst.h>

namespace Media::Adapters {

    void GstAdapter::padAddedHandler (GstElement *src, GstPad *new_pad, const GstAdapterContext &data) {
        GstPad *sink_pad = gst_element_get_static_pad (data.convert.get(), "sink");
        GstPadLinkReturn ret;
        GstCaps *new_pad_caps = NULL;
        GstStructure *new_pad_struct = NULL;
        const gchar *new_pad_type = NULL;

        g_print ("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

        /* If our converter is already linked, we have nothing to do here */
        if (gst_pad_is_linked (sink_pad)) {
        g_print ("We are already linked. Ignoring.\n");
        goto exit;
        }

        /* Check the new pad's type */
        new_pad_caps = gst_pad_get_current_caps (new_pad);
        new_pad_struct = gst_caps_get_structure (new_pad_caps, 0);
        new_pad_type = gst_structure_get_name (new_pad_struct);
        if (!g_str_has_prefix (new_pad_type, "audio/x-raw")) {
        g_print ("It has type '%s' which is not raw audio. Ignoring.\n", new_pad_type);
        goto exit;
        }

        /* Attempt the link */
        ret = gst_pad_link (new_pad, sink_pad);
        if (GST_PAD_LINK_FAILED (ret)) {
        g_print ("Type is '%s' but link failed.\n", new_pad_type);
        } else {
        g_print ("Link succeeded (type '%s').\n", new_pad_type);
        }

        exit:
        /* Unreference the new pad's caps, if we got them */
        if (new_pad_caps != NULL)
        gst_caps_unref (new_pad_caps);

        /* Unreference the sink pad */
        gst_object_unref (sink_pad);
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
        context_.convert = Wrappers::GstElementPtr(gst_element_factory_make ("audioconvert", "convert"));
        context_.resample = Wrappers::GstElementPtr(gst_element_factory_make ("audioresample", "resample"));
        context_.sink = Wrappers::GstElementPtr(gst_element_factory_make ("autoaudiosink", "sink"));

        /* Create the empty pipeline */
        context_.pipeline = Wrappers::GstElementPtr(gst_pipeline_new ("test-pipeline"));

        if (!context_.pipeline || !context_.source || !context_.convert || !context_.resample || !context_.sink) {
            g_printerr ("Not all elements could be created.\n");
            
            return;
        }

        /* Build the pipeline. Note that we are NOT linking the source at this
        * point. We will do it later. */
        gst_bin_add_many (GST_BIN (context_.pipeline.get()), context_.source.get(), context_.convert.get(), context_.resample.get(), context_.sink.get(), NULL);

        if (!gst_element_link_many (context_.convert.get(), context_.resample.get(), context_.sink.get(), NULL)) {
            g_printerr ("Elements could not be linked.\n");
            gst_object_unref (context_.pipeline.get());

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
        
        if (!context_.pipeline || !context_.source || !context_.convert || !context_.resample || !context_.sink) {
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
            gst_object_unref (context_.pipeline.get());
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

            gst_message_unref (message_.get());
            }
        } while (!terminate || !stop_token.stop_requested());

        /* Free resources */
        gst_object_unref (bus_.get());
        gst_element_set_state (context_.pipeline.get(), GST_STATE_NULL);
        gst_object_unref (context_.pipeline.get());
    }
}
