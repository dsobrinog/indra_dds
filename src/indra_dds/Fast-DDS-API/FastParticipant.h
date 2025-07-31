#pragma once

#include <IParticipant.h>

class IPublisher;
class ISubscriber;
class ITopic;

namespace eprosima { namespace fastdds { namespace dds { class DomainParticipant; }}}

namespace ddscomms{

    class FastParticipant : public IParticipant
    {
    public:

        FastParticipant();
        ~FastParticipant();

        void Enable() override;
        void Disable() override;
        inline bool IsEnabled() const override { return enabled; }

        IPublisher*     create_pub()    override;
        ISubscriber*    create_sub()    override;
        ITopic*         create_topic()  override;

    protected:
        virtual void OnInit() override;
        virtual void OnDestroy() override;

        eprosima::fastdds::dds::DomainParticipant* participant = nullptr;
    };
}

