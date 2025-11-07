#pragma once


class IDomainParticipant
{
public:
    IDomainParticipant(int domain_id) :_domain_id(domain_id)
    {
        RequestParticipantCreation();
    } 
    virtual ~IDomainParticipant() = default;

    virtual bool RequestParticipantCreation() = 0;

    virtual bool CreatePublisher() = 0;
    virtual bool CreateSubscriber() = 0;

    virtual bool CreatePublisherControl() = 0;
    virtual bool CreateSubscriberControl() = 0;

    virtual void set_expected_subs(int expected){
        messages_per_cycle = expected;
    }
    virtual bool is_matched() = 0;
    
    int messages_per_cycle = 0;

protected:
    int _domain_id = -1;
};
