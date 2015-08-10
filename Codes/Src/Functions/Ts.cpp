#include "SystemInclude.h"
#include "Common.h"

#include "Nit.h"
#include "Ts.h"

using namespace std;

/**********************class Segment**********************/
Segment::Segment(const SectionBase& section, size_t segmentSize)
{
    /* refer to <ISO/IEC 13818-1> "pointer_field" */    
    size_t size = section.GetCodesSize() + 1;           /* 1 bytes for pointer_field and */
    size = size + (segmentSize - (size % segmentSize)); /* size must be times of segmentSize */
    buffer.reset(new uchar_t[size], UcharDeleter());

    buffer.get()[0] = 0x0; //pointer_field
    section.MakeCodes(buffer.get() + 1, size - 1);

    for (uchar_t *ptr = buffer.get(); ptr < buffer.get() + size; ptr = ptr + segmentSize)
    {
        segments.push_back(ptr);
    }
    size_t tail = size % segmentSize;
    if ((tail) != 0)
    {
        uchar_t* ptr = segments.back();
        memset(ptr + segmentSize - tail, 0xff, tail);
    }
}

Segment::iterator Segment::begin()
{
    return segments.begin();
}

Segment::iterator Segment::end()
{
    return segments.end();
}

/**********************class Ts**********************/
size_t Ts::GetCodesSize(const SectionBase& section) const
{
    size_t sectionSize = section.GetCodesSize() + 1;
    size_t segmentSize = TsPacketSize - sizeof(transport_packet);
    size_t segmentNumber = (sectionSize + 1) / segmentSize;

    if ((sectionSize % segmentSize) != 0)
    {
        segmentNumber = segmentNumber + 1;
    }

    return (segmentNumber * TsPacketSize);
}

size_t Ts::MakeCodes(const SectionBase& section, uchar_t *buffer, size_t bufferSize) const
{
    size_t segmentSize = TsPacketSize - sizeof(transport_packet);
    Segment segment(section, segmentSize);
    uchar_t *ptr = buffer;

    assert(GetCodesSize(section) <= bufferSize);
    for (auto iter = segment.begin(); iter != segment.end(); ++iter)
    {
        ptr = ptr + Write8(ptr, 0x47);
        /* transport_error_indicator(1 bit), payload_unit_start_indicator(1 bit), 
           transport_priority(1 bit), PID(13 bits)
           transport_error_indicator = 0;
           transport_priority = 0;
         */
        uint16_t startIndicator = iter == segment.begin() ? 1 : 0;
        ptr = ptr + Write16(ptr, (startIndicator << 14) | section.GetPid());
        /* transport_scrambling_control[2]='00';
		   adaptation_field_control[2]='01';
		   continuity_counter[4] = '0000';
		*/
        ptr = ptr + Write8(ptr, 1 << 4);
        ptr = ptr + MemCopy(ptr, segmentSize, *iter, segmentSize);
    }

    return (ptr - buffer);
}