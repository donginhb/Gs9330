#ifndef _Descriptor_h_
#define _Descriptor_h_

#include "Component.h"
/**********************class UcharDescriptor**********************/
class UcharDescriptor: public Descriptor
{
public: 
    /* Constructor,  create instance by coded value,
    theData = {tag, length, ... }
    */
    UcharDescriptor(uchar_t *theData) 
        : data(new uchar_t[theData[1] + 2], UcharDeleter())
    { 
        memcpy(data.get(), theData, theData[1] + 2);
    }

    virtual ~UcharDescriptor()
    {}

    size_t  GetCodesSize() const;
    uchar_t GetTag() const;
    size_t  MakeCodes(uchar_t *buffer, size_t bufferSize) const;

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;

protected:
    std::shared_ptr<uchar_t> data;
};

class DescriptorCreator
{
public:
    DescriptorCreator() {}
    virtual ~DescriptorCreator() {}

    /* Create descriptor from string. */
    virtual Descriptor* CreateInstance(std::string &strData) { return nullptr; }

    /* Create descriptor from binary (for example .ts file). */
    virtual Descriptor* CreateInstance(uchar_t *data) { return nullptr; }
};

/**********************class NetworkNameDescriptor**********************/
/* network_name_descriptor */
class NetworkNameDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x40};

    NetworkNameDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class NetworkNameDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x40);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();        

        std::string networkName;
        data =  data + ConvertUtf8ToString(data, networkName, descriptorLenght);

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)networkName.size());
        ptr = ptr + WriteBuffer(ptr, networkName);

        return new NetworkNameDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new NetworkNameDescriptor(data); 
    }
};

/**********************class ServiceListDescriptor**********************/
/* service_list_descriptor */
class ServiceListDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x41};

    ServiceListDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ServiceListDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x41);

        assert(descriptorLenght % 3 == 0);
        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();
        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);

        for (size_t i = 0; i < descriptorLenght; i = i + 3)
        {
            uint16_t serviceId;
            uchar_t  serviceType;

            data = data + ConvertHexStrToInt(data, serviceId);
            data = data + ConvertHexStrToInt(data, serviceType);

            ptr = ptr + WriteBuffer(ptr, serviceId);
            ptr = ptr + WriteBuffer(ptr, serviceType);
        }

        return new ServiceListDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ServiceListDescriptor(data); 
    }
};

/**********************class StuffingDescriptor**********************/
/* stuffing_descriptor */
class StuffingDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x42};
    StuffingDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class StuffingDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x42);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();
        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);

        ptr = ptr + ConvertStrToIntStr(data, descriptorLenght * 2, ptr);

        return new ServiceListDescriptor(data);
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new StuffingDescriptor(data); 
    }
};

/**********************class SatelliteDeliverySystemDescriptor**********************/
/* satellite_delivery_system_descriptor */
class SatelliteDeliverySystemDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x43};
    SatelliteDeliverySystemDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class SatelliteDeliverySystemDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x43);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();
        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);

        uint32_t frequency;
        uint16_t orbitalPosition;
        uchar_t  weastEastFlag; //west_east_flag + polarization + roll off/"00" + modulation_system + modulation_type
        uint32_t symbolRate;    //symbol_rate + FEC_inner

        data = data + ConvertHexStrToInt(data, frequency);
        data = data + ConvertHexStrToInt(data, orbitalPosition);
        data = data + ConvertHexStrToInt(data, weastEastFlag);
        data = data + ConvertHexStrToInt(data, symbolRate);

        ptr = ptr + WriteBuffer(ptr, frequency);
        ptr = ptr + WriteBuffer(ptr, orbitalPosition);
        ptr = ptr + WriteBuffer(ptr, weastEastFlag);
        ptr = ptr + WriteBuffer(ptr, symbolRate);

        return new SatelliteDeliverySystemDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new SatelliteDeliverySystemDescriptor(data); 
    }
};

/**********************class CableDeliverySystemDescriptor**********************/
/* cable_delivery_system_descriptor  */
class CableDeliverySystemDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x44};
    CableDeliverySystemDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class CableDeliverySystemDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x44);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();
        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);

        uint32_t frequency;
        uint16_t reserved; //reserved_future_use + FEC_outer
        uchar_t  modulation; 
        uint32_t symbolRate;   

        data = data + ConvertHexStrToInt(data, frequency);
        data = data + ConvertHexStrToInt(data, reserved);
        data = data + ConvertHexStrToInt(data, modulation);
        data = data + ConvertHexStrToInt(data, symbolRate);

        ptr = ptr + WriteBuffer(ptr, frequency);
        ptr = ptr + WriteBuffer(ptr, reserved);
        ptr = ptr + WriteBuffer(ptr, modulation);
        ptr = ptr + WriteBuffer(ptr, symbolRate);

        return new CableDeliverySystemDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new CableDeliverySystemDescriptor(data); 
    }
};

/**********************class NetworkNameDescriptor**********************/
/* bouquet_name_descriptor */
class BouquetNameDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x47};
    BouquetNameDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class BouquetNameDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x47);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();        

        std::string bouquetName;
        data =  data + ConvertUtf8ToString(data, bouquetName, descriptorLenght);

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)bouquetName.size());
        ptr = ptr + WriteBuffer(ptr, bouquetName);

        return new BouquetNameDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new BouquetNameDescriptor(data); 
    }
};

/**********************class ServiceDescriptor**********************/
/* service_descriptor  */
class ServiceDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x48};
    ServiceDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ServiceDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x48);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, 0);  //fill a '0' as pad. write value at end of this function.

        uchar_t serviceType, serviceProviderNameLength, serviceNameLength;
        std::string serviceProviderName, serviceName;        

        data = data + ConvertHexStrToInt(data, serviceType);
        data = data + ConvertHexStrToInt(data, serviceProviderNameLength);
        data = data + ConvertUtf8ToString(data, serviceProviderName, serviceProviderNameLength);
        data = data + ConvertHexStrToInt(data, serviceNameLength);
        data = data + ConvertUtf8ToString(data, serviceName, serviceNameLength);        

        ptr = ptr + WriteBuffer(ptr, serviceType);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)serviceProviderName.size());
        ptr = ptr + WriteBuffer(ptr, serviceProviderName);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)serviceName.size());
        ptr = ptr + WriteBuffer(ptr, serviceName);

        //rewrite the descripor length.
        WriteBuffer(buffer.get() + 1, uchar_t(ptr - buffer.get() - 2));

        return new ServiceDescriptor(buffer.get());  
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ServiceDescriptor(data); 
    }
};

/**********************class LinkageDescriptor**********************/
/* linkage_descriptor. */
class LinkageDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x4A};
    LinkageDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class LinkageDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x4A);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);

        uint16_t tsId;
        uint16_t onId;
        uint16_t serviceId;
        uchar_t  linkageType;

        data = data + ConvertHexStrToInt(data, tsId);
        data = data + ConvertHexStrToInt(data, onId);
        data = data + ConvertHexStrToInt(data, serviceId);
        data = data + ConvertHexStrToInt(data, linkageType);  

        ptr = ptr + WriteBuffer(ptr, tsId);
        ptr = ptr + WriteBuffer(ptr, onId);
        ptr = ptr + WriteBuffer(ptr, serviceId);
        ptr = ptr + WriteBuffer(ptr, linkageType);

        if (linkageType == 0x08)
        {
            uchar_t value8; //hand-over_type + reserved_future_use + origin_type
            data = data + ConvertHexStrToInt(data, value8);
            ptr = ptr + WriteBuffer(ptr, value8);
            uchar_t handOverType = (value8 >> 4) & 0xf;
            if (handOverType == 0x1 || handOverType == 0x2 || handOverType == 0x3)
            {
                uint16_t netId;
                data = data + ConvertHexStrToInt(data, netId);
                ptr = ptr + WriteBuffer(ptr, netId);
            }

            uchar_t originType = value8 & 0x1;;
            if (originType == 0x0)
            {
                uint16_t initialServiceId;
                data = data + ConvertHexStrToInt(data, initialServiceId);
                ptr = ptr + WriteBuffer(ptr, initialServiceId);
            }
        }
        else if (linkageType == 0x0D)
        {
            uint16_t targetEventId;
            data = data + ConvertHexStrToInt(data, targetEventId);
            ptr = ptr + WriteBuffer(ptr, targetEventId);

            uchar_t value8; //target_listed + event_simulcast + reserved
            data = data + ConvertHexStrToInt(data, value8);
            ptr = ptr + WriteBuffer(ptr, value8);
        }
        else if (linkageType == 0x0E)
        { 
            uchar_t loopLength;
            data = data + ConvertHexStrToInt(data, loopLength);
            ptr = ptr + WriteBuffer(ptr, loopLength);
            uchar_t *endPtr = data + loopLength;
            while(data < endPtr)
            {
                //  target_listed + event_simulcast + link_type + target_id_type 
                //+ original_network_id_flag + service_id_flag
                uchar_t value8; 
                data = data + ConvertHexStrToInt(data, value8);
                ptr = ptr + WriteBuffer(ptr, value8);

                uchar_t targetIdType = (value8 >> 4) & 0x2;
                uchar_t originalNetworkIdFlag = (value8 >> 1) & 0x1;
                uchar_t serviceIdFlag = value8 & 0x1;
                if (targetIdType == 0x3)
                {
                    uint16_t userDefinedId;
                    data = data + ConvertHexStrToInt(data, userDefinedId);
                    ptr = ptr + WriteBuffer(ptr, userDefinedId);
                }
                else
                {
                    if (targetIdType == 0x1)
                    {
                        uint16_t targetTransportStreamId;
                        data = data + ConvertHexStrToInt(data, targetTransportStreamId);
                        ptr = ptr + WriteBuffer(ptr, targetTransportStreamId);
                    }
                    if (originalNetworkIdFlag != 0)
                    {
                        uint16_t targetOriginalNetworkId;
                        data = data + ConvertHexStrToInt(data, targetOriginalNetworkId);
                        ptr = ptr + WriteBuffer(ptr, targetOriginalNetworkId);
                    }
                    if (serviceIdFlag != 0)
                    {
                        uint16_t targetServiceId;
                        data = data + ConvertHexStrToInt(data, targetServiceId);
                        ptr = ptr + WriteBuffer(ptr, targetServiceId);
                    }
                }
            }
        }

        uchar_t privateDataTypeLength = descriptorLenght - (ptr - (buffer.get() + 2));
        ptr = ptr + ConvertStrToIntStr(data, privateDataTypeLength * 2, ptr);

        assert(ptr - buffer.get() <= UCHAR_MAX);
        return new LinkageDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new LinkageDescriptor(data); 
    }
};

/**********************class ShortEventDescriptor**********************/
/* short_event_descriptor */
class ShortEventDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x4D};
    ShortEventDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ShortEventDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x4D);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, 0);  //fill a '0' as pad. write value at end of this function.

        uchar_t languageCode[3];
        uchar_t eventNameLength;
        std::string eventName;
        uchar_t textLength;
        std::string text;
        data = data + MemCopy(languageCode, 3, data, 3);
        data = data + ConvertHexStrToInt(data, eventNameLength);
        data = data + ConvertUtf8ToString(data, eventName, eventNameLength);
        data = data + ConvertHexStrToInt(data, textLength);
        data = data + ConvertUtf8ToString(data, text, textLength);     

        ptr = ptr + MemCopy(ptr, 3, languageCode, 3);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)eventName.size());
        ptr = ptr + WriteBuffer(ptr, eventName);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)text.size());
        ptr = ptr + WriteBuffer(ptr, text);

        //rewrite the descripor length.
        WriteBuffer(buffer.get() + 1, uchar_t(ptr - buffer.get() - 2));

        return new ShortEventDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ShortEventDescriptor(data); 
    }
};

/**********************class ExtendedEventDescriptor**********************/
/* extended_event_descriptor */
class ExtendedEventDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x4E};
    ExtendedEventDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ExtendedEventDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        assert(tag == 0x4E);
        data = data + ConvertHexStrToInt(data, descriptorLenght);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, 0);  //fill a '0' as pad. write value at end of this function.

        uchar_t descriptorNumber;  //descriptor_number + last_descriptor_number
        uchar_t languageCode[3];
        uchar_t lengthOfItem;      
        uchar_t textLength;        
        std::string text;

        data = data + ConvertHexStrToInt(data, descriptorNumber);
        ptr = ptr + WriteBuffer(ptr, descriptorNumber);

        data = data + MemCopy(languageCode, 3, data, 3);
        ptr = ptr + MemCopy(ptr, 3, languageCode, 3);

        data = data + ConvertHexStrToInt(data, lengthOfItem);
        uchar_t *ptrToLengthOfItem = ptr;  //save the ptr of "length_of_items" field;
        ptr = ptr + WriteBuffer(ptr, 0);

        int leftByteNumber = lengthOfItem;
        while (leftByteNumber > 0)
        {
            uchar_t itemDescriptionLength, itemLength;
            std::string itemDescriptionChar, itemChar;

            data = data + ConvertHexStrToInt(data, itemDescriptionLength);
            data = data + ConvertUtf8ToString(data, itemDescriptionChar, itemDescriptionLength);
            ptr = ptr + WriteBuffer(ptr, (uchar_t)itemDescriptionChar.size());
            ptr = ptr + WriteBuffer(ptr, itemDescriptionChar);

            data = data + ConvertHexStrToInt(data, itemLength);
            data = data + ConvertUtf8ToString(data, itemChar, itemLength);
            ptr = ptr + WriteBuffer(ptr, (uchar_t)itemChar.size());
            ptr = ptr + WriteBuffer(ptr, itemChar);

            leftByteNumber = leftByteNumber - 2 - itemDescriptionLength - itemLength;
        }
        WriteBuffer(ptrToLengthOfItem, (uchar_t)(ptr - ptrToLengthOfItem - 1));
        assert(leftByteNumber == 0);

        data = data + ConvertHexStrToInt(data, textLength);
        data = data + ConvertUtf8ToString(data, text, textLength);

        ptr = ptr + WriteBuffer(ptr, (uchar_t)text.size());
        ptr = ptr + WriteBuffer(ptr, text);

        //rewrite the descripor length.
        WriteBuffer(buffer.get() + 1, uchar_t(ptr - buffer.get() - 2));

        return new ExtendedEventDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ExtendedEventDescriptor(data); 
    }
};

/**********************class TimeShiftedEventDescriptor**********************/
/* time_shifted_event_descriptor */
class TimeShiftedEventDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x4F};
    TimeShiftedEventDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class TimeShiftedEventDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x4F);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght); 
              
        uint16_t referenceServiceId, referenceEventId;
        data = data + ConvertHexStrToInt(data, referenceServiceId);
        data = data + ConvertHexStrToInt(data, referenceEventId);

        ptr = ptr + WriteBuffer(ptr, referenceServiceId);
        ptr = ptr + WriteBuffer(ptr, referenceEventId); 

        return new TimeShiftedEventDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new TimeShiftedEventDescriptor(data); 
    }
};

/**********************class ComponentDescriptor**********************/
/* component_descriptor */
class ComponentDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x50};
    ComponentDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ComponentDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x50);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght); 
              
        uchar_t streamContent, componentType, componentTag;
        data = data + ConvertHexStrToInt(data, streamContent);
        data = data + ConvertHexStrToInt(data, componentType);
        data = data + ConvertHexStrToInt(data, componentTag);

        ptr = ptr + WriteBuffer(ptr, streamContent);
        ptr = ptr + WriteBuffer(ptr, componentType);
        ptr = ptr + WriteBuffer(ptr, componentTag); 

        uchar_t languageCode[3];
        std::string text;
        data = data + MemCopy(languageCode, 3, data, 3);
        data = data + ConvertUtf8ToString(data, text, descriptorLenght - 6);

        ptr = ptr + MemCopy(ptr, 3, languageCode, 3);
        ptr = ptr + WriteBuffer(ptr, text);

        return new ComponentDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ComponentDescriptor(data); 
    }
};

/**********************class CaIdentifierDescriptor**********************/
/* CA_identifier_descriptor */
class CaIdentifierDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x53};
    CaIdentifierDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class CaIdentifierDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x53);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght); 

        while (ptr < buffer.get() + 2 + descriptorLenght)
        {
            uint16_t caSystemId;
            data = data + ConvertHexStrToInt(data, caSystemId);
            ptr = ptr + WriteBuffer(ptr, caSystemId);
        }        

        return new CaIdentifierDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new CaIdentifierDescriptor(data); 
    }
};

/**********************class ContentDescriptor**********************/
/* content_descriptor */
class ContentDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x54};
    ContentDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ContentDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x54);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght); 

        while (ptr < buffer.get() + 2 + descriptorLenght)
        {
            uchar_t content_nibble_level, user_byte;
            data = data + ConvertHexStrToInt(data, content_nibble_level);
            data = data + ConvertHexStrToInt(data, user_byte);

            ptr = ptr + WriteBuffer(ptr, content_nibble_level);
            ptr = ptr + WriteBuffer(ptr, user_byte);
        }        

        return new ContentDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ContentDescriptor(data); 
    }
};

/**********************class ParentalRatingDescriptor**********************/
/* parental_rating_descriptor */
class ParentalRatingDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x55};
    ParentalRatingDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ParentalRatingDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x55);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght); 

        while (ptr < buffer.get() + 2 + descriptorLenght)
        {
            uchar_t countryCode[3];
            uchar_t rating;
            data = data + MemCopy(countryCode, 3, data, 3);
            data = data + ConvertHexStrToInt(data, rating);

            data = data + MemCopy(ptr, 3, countryCode, 3);
            ptr = ptr + WriteBuffer(ptr, rating);
        }        

        return new ParentalRatingDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new ParentalRatingDescriptor(data); 
    }
};

/**********************class TerrestrialDeliverySystemDescriptor**********************/
/* terrestrial_delivery_system_descriptor */
class TerrestrialDeliverySystemDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x5A};
    TerrestrialDeliverySystemDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class TerrestrialDeliverySystemDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x5A);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);

        uchar_t  value8;
        uint16_t value16;
        uint32_t value32;
        data = data + ConvertHexStrToInt(data, value32); //centre_frequency
        ptr = ptr + WriteBuffer(ptr, value32);

        //bandwidth 3 + priority 1 + Time_Slicing_indicator 1 + MPE-FEC_indicator 1 + reserved_future_use 2
        data = data + ConvertHexStrToInt(data, value8); 
        ptr = ptr + WriteBuffer(ptr, value8);

        //constellation 2 + hierarchy_information 3 + code_rate-HP_stream 3 + code_rate-LP_stream 3 
        // + guard_interval 2 + transmission_mode 2 + other_frequency_flag 1
        data = data + ConvertHexStrToInt(data, value16); 
        ptr = ptr + WriteBuffer(ptr, value16);

        //reserved_future_use 32
        data = data + ConvertHexStrToInt(data, value32); 
        ptr = ptr + WriteBuffer(ptr, value32);
        
        return new TerrestrialDeliverySystemDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new TerrestrialDeliverySystemDescriptor(data); 
    }
};

/**********************class MultilingualNetworkNameDescriptor**********************/
/* multilingual_network_name_descriptor */
class MultilingualNetworkNameDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x5B};
    MultilingualNetworkNameDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class MultilingualNetworkNameDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x5B);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, 0);  //fill a '0' as pad. write value at end of this function.

        int leftByteNumber = descriptorLenght;
        while (leftByteNumber > 0)
        {
            uchar_t languageCode[3];
            data = data + MemCopy(languageCode, 3, data, 3);
            uchar_t networkNameLength;
            std::string text;
            data = data + ConvertHexStrToInt(data, networkNameLength);
            data = data + ConvertUtf8ToString(data, text, networkNameLength);

            ptr = ptr + MemCopy(ptr, 3, languageCode, 3);
            ptr = ptr + WriteBuffer(ptr, (uchar_t)text.size());
            ptr = ptr + WriteBuffer(ptr, text);

            leftByteNumber = leftByteNumber - 4 - networkNameLength;
        }

        //rewrite the descripor length.
        WriteBuffer(buffer.get() + 1, uchar_t(ptr - buffer.get() - 2));

        return new MultilingualNetworkNameDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new MultilingualNetworkNameDescriptor(data); 
    }
};

/**********************class MultilingualComponentDescriptor**********************/
/* multilingual_component_descriptor */
class MultilingualComponentDescriptor: public UcharDescriptor
{
public:
    enum: uchar_t {Tag  = 0x5E};
    MultilingualComponentDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class MultilingualComponentDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x5E);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, 0);  //fill a '0' as pad. write value at end of this function.  

        uchar_t componentTag;
        data = data + ConvertHexStrToInt(data, componentTag);
        ptr = ptr + WriteBuffer(ptr, componentTag);

        int leftByteNumber = descriptorLenght - 1;
        while (leftByteNumber > 0)
        {
            uchar_t languageCode[3];
            data = data + MemCopy(languageCode, 3, data, 3);
            uchar_t textDescriptionLength;
            std::string text;
            data = data + ConvertHexStrToInt(data, textDescriptionLength);
            data = data + ConvertUtf8ToString(data, text, textDescriptionLength);

            ptr = ptr + MemCopy(ptr, 3, languageCode, 3);
            ptr = ptr + WriteBuffer(ptr, (uchar_t)text.size());
            ptr = ptr + WriteBuffer(ptr, text);

            leftByteNumber = leftByteNumber - 4 - textDescriptionLength;
        }

        //rewrite the descripor length.
        WriteBuffer(buffer.get() + 1, uchar_t(ptr - buffer.get() - 2));

        return new MultilingualComponentDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new MultilingualComponentDescriptor(data); 
    }
};

/**********************class PrivateDataSpecifierDescriptor**********************/
/* private_data_specifier_descriptor  */
class PrivateDataSpecifierDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x5F};
    PrivateDataSpecifierDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class PrivateDataSpecifierDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x5F);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);  

        uint32_t value32;
        data = data + ConvertHexStrToInt(data, value32);
        ptr = ptr + WriteBuffer(ptr, value32);

        return new PrivateDataSpecifierDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new PrivateDataSpecifierDescriptor(data); 
    }
};

/**********************class FrequencyListDescriptor**********************/
/* frequency_list_descriptor  */
class FrequencyListDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x62};
    FrequencyListDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class FrequencyListDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x62);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);  

        uchar_t reservedFutureUse; //reserved_future_use + coding_type
        data = data + ConvertHexStrToInt(data, reservedFutureUse);
        ptr = ptr + WriteBuffer(ptr, reservedFutureUse);

        while (ptr < buffer.get() + 2 + descriptorLenght)
        {
            uint32_t centreFrequency;
            data = data + ConvertHexStrToInt(data, centreFrequency);
            ptr = ptr + WriteBuffer(ptr, centreFrequency);
        }

        return new FrequencyListDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new FrequencyListDescriptor(data); 
    }
};

/**********************class DataBroadcastDescriptor**********************/
/* data_broadcast_descriptor  */
class DataBroadcastDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x64};
    DataBroadcastDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class DataBroadcastDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x64);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);  

        uint16_t dataBroadcastId;
        uchar_t componentTag, selectorLength;        
        data = data + ConvertHexStrToInt(data, dataBroadcastId);
        data = data + ConvertHexStrToInt(data, componentTag);
        data = data + ConvertHexStrToInt(data, selectorLength);

        ptr = ptr + WriteBuffer(ptr, dataBroadcastId);
        ptr = ptr + WriteBuffer(ptr, componentTag);
        ptr = ptr + WriteBuffer(ptr, selectorLength);  

        /***/
        uchar_t languageCode[3];
        data = data + MemCopy(languageCode, 3, data, 3);
        uchar_t textLength;
        std::string text;
        data = data + ConvertHexStrToInt(data, textLength);
        data = data + ConvertUtf8ToString(data, text, textLength);

        ptr = ptr + MemCopy(ptr, 3, languageCode, 3);
        ptr = ptr + WriteBuffer(ptr, (uchar_t)text.size());
        ptr = ptr + WriteBuffer(ptr, text);

        return new DataBroadcastDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new DataBroadcastDescriptor(data); 
    }
};

/**********************class ExtensionDescriptor**********************/
/* extension descriptor  */
class ExtensionDescriptor: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x7F};
    ExtensionDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class ExtensionDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght, descriptorTagExtension;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        data = data + ConvertHexStrToInt(data, descriptorTagExtension);
        assert(tag == 0x7F);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght); 
        ptr = ptr + WriteBuffer(ptr, descriptorTagExtension);  

        if (descriptorTagExtension  == 0x04)
        {
            //T2_delivery_system_descriptor
            uchar_t  value8;
            uint16_t value16;
            uint32_t value32;

            //plp_id
            data = data + ConvertHexStrToInt(data, value8);
            ptr = ptr + WriteBuffer(ptr, value8);

            //T2_system_id
            data = data + ConvertHexStrToInt(data, value16);
            ptr = ptr + WriteBuffer(ptr, value16);

            if (descriptorLenght > 4)
            {
                //SISO/MISO + bandwidth + reserved_future_use + guard_interval
                // + transmission_mode + other_frequency_flag + tfs_flag
                data = data + ConvertHexStrToInt(data, value16);
                ptr = ptr + WriteBuffer(ptr, value16);

                uint16_t tfsFlag = value16 & 0x1;

                while (ptr < buffer.get() + 2 + descriptorLenght)
                {
                    //cell_id
                    data = data + ConvertHexStrToInt(data, value16);
                    ptr = ptr + WriteBuffer(ptr, value16);

                    if (tfsFlag == 1)
                    {
                        //frequency_loop_length
                        data = data + ConvertHexStrToInt(data, value8);
                        ptr = ptr + WriteBuffer(ptr, value8);
                        for (uint_t i = 0; i < value8; i = i + 4)
                        {
                            //centre_frequency
                            data = data + ConvertHexStrToInt(data, value32);
                            ptr = ptr + WriteBuffer(ptr, value32);
                        }
                    }
                    else
                    {
                        //centre_frequency
                        data = data + ConvertHexStrToInt(data, value32);
                        ptr = ptr + WriteBuffer(ptr, value32);
                    }

                    //subcell_info_loop_length
                    data = data + ConvertHexStrToInt(data, value8);
                    ptr = ptr + WriteBuffer(ptr, value8);
                    for (uint_t i = 0; i < value8; i = i + 4)
                    {
                        //cell_id_extension
                        data = data + ConvertHexStrToInt(data, value8);
                        ptr = ptr + WriteBuffer(ptr, value8);

                        //transposer_frequency
                        data = data + ConvertHexStrToInt(data, value32);
                        ptr = ptr + WriteBuffer(ptr, value32);
                    }
                }
            }
        }

        return new DataBroadcastDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new DataBroadcastDescriptor(data); 
    }
};

/**********************class UserdefinedDscriptor83**********************/
/* user defined dscriptor, LCN 和音量补偿  */
class UserdefinedDscriptor83: public UcharDescriptor
{
public: 
    enum: uchar_t {Tag  = 0x83};
    UserdefinedDscriptor83(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class UserdefinedDscriptor83Creator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        assert(tag == 0x83);

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);  

        uint16_t lcn;  // lcn flag + lcn
        uchar_t vc;    // vc falg + vc

        data = data + ConvertHexStrToInt(data, lcn);
        data = data + ConvertHexStrToInt(data, vc);
        ptr = ptr + WriteBuffer(ptr, lcn);
        ptr = ptr + WriteBuffer(ptr, vc);

        return new UserdefinedDscriptor83(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new UserdefinedDscriptor83(data); 
    }
};

/**********************class UndefinedDescriptor**********************/
/* user defined dscriptor, both tag and data are undefined  */
class UndefinedDescriptor: public UcharDescriptor
{
public:
    UndefinedDescriptor(uchar_t *data)
        : UcharDescriptor(data)
    {}
};

class UndefinedDescriptorCreator: public DescriptorCreator
{
public: 
    Descriptor* CreateInstance(std::string &strData)
    {
        uchar_t *data = (uchar_t *)strData.c_str();
        uchar_t tag, descriptorLenght;
        data = data + ConvertHexStrToInt(data, tag);
        data = data + ConvertHexStrToInt(data, descriptorLenght);
        if (tag < 0x80)
        {
            /* tag of customized descriptor must >= than 0x80 */
            return nullptr;
        }

        std::shared_ptr<uchar_t> buffer(new uchar_t[UCHAR_MAX], UcharDeleter());
        uchar_t *ptr = buffer.get();

        ptr = ptr + WriteBuffer(ptr, tag);
        ptr = ptr + WriteBuffer(ptr, descriptorLenght);  
        ptr = ptr + ConvertStrToIntStr(data, descriptorLenght * 2, ptr);

        return new UndefinedDescriptor(buffer.get());
    }

    Descriptor* CreateInstance(uchar_t *data) 
    { 
        return new UndefinedDescriptor(data); 
    }
};

/**********************class Descriptors**********************/
class Descriptors: public Components
{
public:
    typedef Components MyBase;

    void AddDescriptor(std::shared_ptr<Descriptor> discriptor);

    /* the following function is provided just for debug */
    void Put(std::ostream& os) const;
};

/**********************class DescriptorFactory**********************/
class DescriptorFactory
{
public:
    /* Create descriptor from string */
    Descriptor* DescriptorFactory::Create(std::string data)
    {
        uchar_t tag;
        ConvertHexStrToInt((uchar_t*)data.c_str(), tag);
        auto iter = creators.find(tag);
        if (iter == creators.end())
        {
            UndefinedDescriptorCreator creator;
            return creator.CreateInstance(data);
        }

        std::shared_ptr<DescriptorCreator> creator = iter->second;
        return creator->CreateInstance(data);
    }

    /* Create descriptor from binary (for example .ts file). */
    Descriptor* DescriptorFactory::Create(uchar_t *data)
    {
        auto iter = creators.find(data[0]);
        if (iter == creators.end())
        {
            UndefinedDescriptorCreator creator;
            return creator.CreateInstance(data);
        }

        std::shared_ptr<DescriptorCreator> creator = iter->second;
        return creator->CreateInstance(data);
    }

    void Register(uchar_t tag, std::shared_ptr<DescriptorCreator> creator)
    {
        creators.insert(make_pair(tag, creator));
    }

    static DescriptorFactory& GetInstance()
    {
        static DescriptorFactory instance;
        return instance;
    }

private:
    DescriptorFactory() { /* do nothing */ }
    std::map<uchar_t, std::shared_ptr<DescriptorCreator>> creators;
};

class AutoRegisterSuite
{
public:
    AutoRegisterSuite(uchar_t tag, std::shared_ptr<DescriptorCreator> creator)
    {
        DescriptorFactory& factory = DescriptorFactory::GetInstance();
        factory.Register(tag, creator);
    }
};

#define DescriptorCreatorRegistration(tag, creator)      \
    static AutoRegisterSuite  JoinName(descriptorCreator, __LINE__)(tag, std::make_shared<creator>())

/* Create descriptor from string */
Descriptor* CreateDescriptor(std::string &data);

/* Create descriptor from binary (for example .ts file). */
Descriptor* CreateDescriptor(uchar_t *data);

#endif