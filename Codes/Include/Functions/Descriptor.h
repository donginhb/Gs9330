#ifndef _Descriptor_h_
#define _Descriptor_h_

#include "Component.h"
/**********************class UcharDescriptor**********************/
class UcharDescriptor: public Descriptor
{
public: 
    UcharDescriptor(): data(nullptr), dataSize(0)
    {}

    UcharDescriptor(uchar_t *theData, size_t theDataSize)
        : data(new uchar_t[theDataSize+1], UcharDeleter()), dataSize(theDataSize)
    {
        /* we allocate 1 more bytes to store '0', just for debug function Put() to
           show the value correctly.
         */
        data.get()[theDataSize] = '\0';
        memcpy(data.get(), theData, theDataSize);
    }

    virtual ~UcharDescriptor()
    {}

    virtual uchar_t GetTag() const = 0;
    size_t GetCodesSize() const;
    size_t MakeCodes(uchar_t *buffer, size_t bufferSize) const;

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;
    void Reset(uchar_t *theData, size_t theDataSizeaSize)
    {
        data.reset(theData);
        dataSize = theDataSizeaSize;
    }

private:
    std::shared_ptr<uchar_t> data;
    size_t dataSize;
};

/**********************class NetworkNameDescriptor**********************/
/* network_name_descriptor */
class NetworkNameDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x40};
    NetworkNameDescriptor(uchar_t *theData, size_t theDataSize)
        : UcharDescriptor(theData, theDataSize)
    {}

    static Descriptor* CreateInstance(uchar_t *data, size_t dataSize)
    {
        return new NetworkNameDescriptor(data, dataSize);
    }
    uchar_t GetTag() const { return Tag; }
};

/**********************class ServiceListDescriptor**********************/
/* service_list_descriptor */
class ServiceListDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x41};
    ServiceListDescriptor()
    {};

    ServiceListDescriptor(uchar_t *theData, size_t theDataSize)
        : UcharDescriptor(theData, theDataSize)
    {}

    void SetData(const std::list<std::pair<uint16_t, uchar_t>>& serviceList)
    {
        size_t size = 3 * serviceList.size();
        uchar_t *buffer = new uchar_t[size];

        uchar_t *ptr = buffer;
        for (auto iter: serviceList)
        {
            ptr = ptr + Write16(ptr, iter.first);
            ptr = ptr + Write8(ptr, iter.second);
        }
        Reset(buffer, size);
    }

    static Descriptor* CreateInstance(uchar_t *data, size_t dataSize)
    {
        return new ServiceListDescriptor(data, dataSize);
    }
    uchar_t GetTag() const { return Tag; }
};

/**********************class StuffingDescriptor**********************/
/* stuffing_descriptor */
class StuffingDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x42};
    StuffingDescriptor(uchar_t *theData, size_t theDataSize)
        : UcharDescriptor(theData, theDataSize)
    {}

    static Descriptor* CreateInstance(uchar_t *data, size_t dataSize)
    {
        return new StuffingDescriptor(data, dataSize);
    }
    uchar_t GetTag() const { return Tag; }
};

/**********************class SatelliteDeliverySystemDescriptor**********************/
/* satellite_delivery_system_descriptor */
class SatelliteDeliverySystemDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x43};
    SatelliteDeliverySystemDescriptor(uchar_t *theData, size_t theDataSize)
        : UcharDescriptor(theData, theDataSize)
    {}

    static Descriptor* CreateInstance(uchar_t *data, size_t dataSize)
    {
        return new SatelliteDeliverySystemDescriptor(data, dataSize);
    }
    uchar_t GetTag() const { return Tag; }
};

/**********************class cable_delivery_system_descriptor**********************/
/* cable_delivery_system_descriptor  */
class CableDeliverySystemDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x44};
    CableDeliverySystemDescriptor()
    {}

    CableDeliverySystemDescriptor(uchar_t *theData, size_t theDataSize)
        : UcharDescriptor(theData, theDataSize)
    {}

    void SetData(uint32_t frequency, uint16_t fecOunter, uchar_t modulation,
                                  uint32_t symbolRate, uint32_t fecInner)
    {
        size_t size = 11;
        uchar_t *buffer = new uchar_t[size];
        uchar_t *ptr = buffer;
        ptr = ptr + Write32(ptr, frequency);
        ptr = ptr + Write16(ptr, (Reserved12Bit << 4) | (fecOunter & 0xf));
        ptr = ptr + Write8(ptr, modulation);
        ptr = ptr + Write32(ptr, (symbolRate << 4) | fecInner);

        Reset(buffer, size);
    }

    static Descriptor* CreateInstance(uchar_t *data, size_t dataSize)
    {
        return new CableDeliverySystemDescriptor(data, dataSize);
    }
    uchar_t GetTag() const { return Tag; }
};

/**********************class UserdefinedDscriptor83**********************/
/* user defined dscriptor  */
class UserdefinedDscriptor83: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x83};
    UserdefinedDscriptor83(uchar_t *theData, size_t theDataSize)
        : UcharDescriptor(theData, theDataSize)
    {}

    static Descriptor* CreateInstance(uchar_t *data, size_t dataSize)
    {
        return new UserdefinedDscriptor83(data, dataSize);
    }
    uchar_t GetTag() const { return Tag; }
};

/**********************class Descriptors**********************/
class Descriptors: public Components
{
public:
    typedef Components MyBase;

    void AddDescriptor(uchar_t tag, uchar_t* data, size_t dataSize);
    size_t MakeCodes(uchar_t *buffer, size_t bufferSize) const;

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;
};

/**********************class DescriptorFactor**********************/
typedef std::function<Descriptor*(uchar_t *data, size_t dataSize)> DescriptorCreator;

class DescriptorFactory
{
public:
    void Register(uchar_t type, DescriptorCreator creator);
    Descriptor* Create(uchar_t type, uchar_t *data, size_t dataSize);

    static DescriptorFactory& GetInstance()
    {
        static DescriptorFactory instance;
        return instance;
    }

private:
    DescriptorFactory() { /* do nothing */ }
    std::map<uchar_t, DescriptorCreator> creatorMap;
};

class AutoRegisterSuite
{
public:
    AutoRegisterSuite(uchar_t type, DescriptorCreator creator)
    {
        DescriptorFactory& factory = DescriptorFactory::GetInstance();
        factory.Register(type, creator);
    }
};


#define DescriptorCreatorRgistration(type, creator)      \
    static AutoRegisterSuite  JoinName(descriptorCreator, __LINE__)(type, creator)

Descriptor* CreateDescriptor(uchar_t type, uchar_t *data, size_t dataSize);

#endif