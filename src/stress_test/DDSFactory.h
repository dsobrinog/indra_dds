#pragma once
#include "abstraction/IPub.h"
#include "abstraction/ISub.h"
#include "abstraction/IPubControl.h"
#include "abstraction/ISubControl.h"

// Definir solo UNA de estas macros durante la compilación
// #define USE_FAST_DDS
// #define USE_CYCLONE_DDS

#ifdef USE_FAST_DDS
    // FAST DDS
    #include "implementation/fast_dds/FastPub.hpp"
    #include "implementation/fast_dds/FastPubControl.hpp"
    #include "implementation/fast_dds/FastSub.hpp"
    #include "implementation/fast_dds/FastSubControl.hpp"
    
    using DefaultPub = FastPub;
    using DefaultPubControl = FastPubControl;
    using DefaultSub = FastSub;
    using DefaultSubControl = FastSubControl;

#elif USE_CYCLONE_DDS
    // Cyclone DDS
    // #include <dds/dds.h>        // C API first
    // #include <dds/dds.hpp>      // C++ API second
    #include "implementation/cyclone_dds/CyclonePub.hpp"
    #include "implementation/cyclone_dds/CyclonePubControl.hpp"
    #include "implementation/cyclone_dds/CycloneSub.hpp"
    #include "implementation/cyclone_dds/CycloneSubControl.hpp"
    
    using DefaultPub = CyclonePub;
    using DefaultPubControl = CyclonePubControl;
    using DefaultSub = CycloneSub;
    using DefaultSubControl = CycloneSubControl;

#elif USE_OPEN_DDS

    #include "implementation/open_dds/OpenPub.hpp"
    #include "implementation/open_dds/OpenPubControl.hpp"
    #include "implementation/open_dds/OpenSub.hpp"
    #include "implementation/open_dds/OpenSubControl.hpp"
    
    using DefaultPub = OpenPub;
    using DefaultPubControl = OpenPubControl;
    using DefaultSub = OpenSub;
    using DefaultSubControl = OpenSubControl;

#elif USE_RTI_DDS
#elif
    #error "Must define either USE_FAST_DDS or USE_CYCLONE_DDS"
#endif

class DDSFactory {
public:

    template <typename T> 
    static std::unique_ptr<IPub<T>> createPublisher() {
        return std::make_unique<DefaultPub>();
    }
    static std::unique_ptr<IPubControl> createPublisherControl() {
        return std::make_unique<DefaultPubControl>();
    }
    static std::unique_ptr<ISub> createSubscriber() {
    return std::make_unique<DefaultSub>();
    }
    static std::unique_ptr<ISubControl> createSubscriberControl() {
        return std::make_unique<DefaultSubControl>();
    }
};