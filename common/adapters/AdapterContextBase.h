#pragma once

#include  "../MediaElement.h"
#include "VideoAdapters.h"


namespace Media::Common::Adapters
{
    class AdapterContextBase
    {
    public:
        AdapterContextBase(Media::Common::MediaElement& mediaElement, AdapterType adapterType);
        ~AdapterContextBase() = default;
        virtual struct AdapterContext =0;
    };
}