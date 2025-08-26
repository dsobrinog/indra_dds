    #include <Core/TypeSupport/ITypeSupport.h>
    #include <fastdds/dds/topic/TypeSupport.hpp>

    namespace indradds
    {

        class FastTypeSupport : public ITypeSupport
        {
        public:
            FastTypeSupport(const eprosima::fastdds::dds::TypeSupport& ts)
                : type_support_(ts) {}

            const eprosima::fastdds::dds::TypeSupport& Get() const { return type_support_; }

        private:
            eprosima::fastdds::dds::TypeSupport type_support_;
        };
    }