#pragma once

#include  "../MediaElement.h"


namespace Media::Common::Adapters
{
    class AdapterContextBase
    {
    public:
        AdapterContextBase(Media::Common::MediaElement& mediaElement, AdapterType adapterType)
        ~AdapterContext() = default;
        virtual typedef struct AdapterContext;
    };
}