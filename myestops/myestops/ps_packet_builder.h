#ifndef MEDIA_CAST_FORMATS_PSPACKET_PS_PACKET_BUILDER_H_
#define MEDIA_CAST_FORMATS_PSPACKET_PS_PACKET_BUILDER_H_

#include <list>
#include <string.h>

#include "ps_defines.h"

namespace media {
namespace mpeg2 {



class PsPacketBuilder {
  public:
    PsPacketBuilder();
    ~PsPacketBuilder();
    
    int PacketIFrame(const uint8_t* src_buff, int src_len, PsPacket* packet, int frame_rate, int compression);
    int PacketPFrame(const uint8_t* src_buff, int src_len, PsPacket* packet);
  private:
    //封装PS_Header
    int PacketPSHeader(uint8_t* dest_buff, int& dest_len, bool is_aligned = false);
    //封装PS_MAP
    int PacketPSMap(uint8_t* dest_buff, int& dest_len, int compression);
    int PacketSystemHeader(uint8_t* dest_buff, int& dest_len);
    //封装视频帧
    int PacketVideoFrame(const uint8_t* src_buff, int src_len, uint8_t* dest_buff, int& dest_len, bool is_frist = true);
     //拆分NAL包
    int GeneratePacketsFromFrame(const uint8_t* src_buff, int src_len, uint8_t* dest_buff, int& dest_len);
  private:
    //system_clock_reference_base
    unsigned int scrb;
    //DTS时标
    unsigned int dts;
    //Video PTS时标
    unsigned int video_pts;
    //码流的Rate
    int frame_rate;
    int use_bytes_size;


  };

} // namespace pspaket
} // namespace media

#endif  // MEDIA_CAST_FORMATS_PSPACKET_PS_PACKET_BUILDER_H_

