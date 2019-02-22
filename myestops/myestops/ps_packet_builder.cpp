#include "ps_packet_builder.h"
//#include "base/logging.h"


namespace media {
namespace mpeg2 {


PsPacketBuilder::PsPacketBuilder()
{
	scrb = 0;
	dts = 0;
	video_pts = 60000;
	frame_rate = 25;
	use_bytes_size = 0;
}

PsPacketBuilder::~PsPacketBuilder()
{
	
}


int PsPacketBuilder::PacketIFrame(const uint8_t* src_buff, int src_len, PsPacket* packet, int re_frame_rate, int compression)
{
	//DCHECK(src_buff);
  //DCHECK(packet);

  int temp_len = 0;
  int total_len = 0;

  if (frame_rate != re_frame_rate) {
		frame_rate = re_frame_rate ;
	}
  
	PacketPSHeader(packet->out_buf, temp_len);
  total_len += temp_len;
  //增加系统头
	PacketSystemHeader(packet->out_buf+total_len, temp_len);
  total_len += temp_len;
	PacketPSMap(packet->out_buf+total_len, temp_len, compression);
  total_len += temp_len;
  
	GeneratePacketsFromFrame(src_buff, src_len, packet->out_buf+total_len, temp_len);
  total_len += temp_len;

  packet->len = total_len;
  use_bytes_size++;
  printf("count:%d\n", use_bytes_size);
	return kReturnOk;
}


int PsPacketBuilder::PacketPFrame(const uint8_t* src_buff, int src_len, PsPacket* packet)
{
  //DCHECK(src_buff);
  //DCHECK(packet);

  int temp_len = 0;
  int total_len = 0;
  
  PacketPSHeader(packet->out_buf, temp_len);
  total_len += temp_len;
  
  GeneratePacketsFromFrame(src_buff, src_len, packet->out_buf+total_len, temp_len);
  total_len += temp_len;

  packet->len = total_len;
	return kReturnOk ;
}

int PsPacketBuilder::PacketPSHeader(uint8_t* dest_buff, int& dest_len, bool is_aligned)
{
  //DCHECK(dest_buff);
  memcpy(dest_buff, kPSHeader , kPSPacketStartCodeLen);
  
  char temp = 0x00;
  scrb = video_pts;
  
  dest_buff[4] = ((0x38&(scrb>>26))|0x44);
  dest_buff[4] = dest_buff[4]|((scrb>>28)&0x03);
  dest_buff[5] = ((scrb>>20)&(0xFF));
  temp = ((scrb>>12)&(0xF8));
  dest_buff[6] = (temp|0x04|((scrb>>13)&0x03));
  dest_buff[7] = ((scrb>>5)&(0xFF));
  temp=((((scrb&0x1f)<<3)&0xf8)|0x04);
  dest_buff[8] = (temp|0x03);
  dest_buff[9] = (0x01);
				
  dest_buff[10] = ((kMuxRate>>14)&0xff);
  dest_buff[11] = ((kMuxRate>>6)&0xff);
  dest_buff[12] = (((kMuxRate<<2)&0xfc)|0x03);

  if (!is_aligned) {
	dest_buff[13] = (0xF8);
    dest_len = kPSHeaderLen;
  }
  else {
	dest_buff[13] = (0xFA);
	dest_buff[14] = (0xFF);
	dest_buff[15] = (0xFF);
    dest_len = kPSHeaderLenAligned;
	printf("22222222\n");
  }
  
	return kReturnOk;
  
}
//封装PS_MAP
int PsPacketBuilder::PacketPSMap(uint8_t* dest_buff, int& dest_len, int compression)
{
  //DCHECK(dest_buff);
  memcpy(dest_buff, kPSMapHeader , kPSPacketStartCodeLen);
  
  dest_buff[4] = 0x00;	
	dest_buff[5] = 0x18;

	dest_buff[6] = 0xE1;
	dest_buff[7] = 0xFF;

	dest_buff[8] = 0x00;
	dest_buff[9] = 0x00;

	dest_buff[10] = 0x00;
	dest_buff[11] = 0x08;

	//element_info
	dest_buff[12] = 0x1B;   //H.264

	dest_buff[13] = 0xE0;
	dest_buff[14] = 0x00;
	dest_buff[15] = 0x06;

	//avc timming and hrd descriptor
	dest_buff[16] = 0x0a;
	dest_buff[17] = 0x04;
	dest_buff[18] = 0x65;
	dest_buff[19] = 0x6e;
	dest_buff[20] = 0x67;
	dest_buff[21] = 0x00;

	dest_buff[22] = 0x90;
	dest_buff[23] = 0xc0;
	dest_buff[24] = 0x00;
	dest_buff[25] = 0x00;

	dest_buff[26] = 0x23;
	dest_buff[27] = 0xb9;
	dest_buff[28] = 0x0f;
	dest_buff[29] = 0x3d;

  dest_len = 30;
  
  return kReturnOk;
}
int PsPacketBuilder::PacketSystemHeader(uint8_t* dest_buff, int& dest_len)
{
  //DCHECK(dest_buff);
  memcpy(dest_buff, kPSSystemHeader , kPSPacketStartCodeLen);
  
  dest_buff[4] = 0x00;	
	dest_buff[5] = 0x0C;

	dest_buff[6] = 0x80;
	dest_buff[7] = 0x1E;

	dest_buff[8] = 0xFF;
	dest_buff[9] = 0xFE;

	dest_buff[10] = 0xE1;
	dest_buff[11] = 0x7F;    

  dest_buff[12] = 0xE0;
	dest_buff[13] = 0xE0;

	dest_buff[14] = 0xD8;
	dest_buff[15] = 0xC0;

	dest_buff[16] = 0xC0;
	dest_buff[17] = 0x20;

  dest_len = 18;
  return kReturnOk;
}
//封装视频帧
int PsPacketBuilder::PacketVideoFrame(const uint8_t* src_buff, int src_len, uint8_t* dest_buff, int& dest_len, bool is_frist)
{
  //DCHECK(src_buff);
  //DCHECK(dest_buff);

  int temp_len = 0;
  if (is_frist) {
    dts = video_pts + OFFSET(frame_rate);
		video_pts = video_pts + OFFSET(frame_rate);
  }

  memcpy(dest_buff, kPSVideoPesHeader , kPSPacketStartCodeLen);

  temp_len = 8 + src_len;
  //PES包的长度
  dest_buff[4] = (temp_len >> 8) & 0xFF;
  dest_buff[5] = temp_len & 0xFF;
  
  dest_buff[6] = 0x88;
  dest_buff[7] = 0x80;
  
  //PTS_DTS_flag = '10';
  dest_buff[8] = 0x05;
  if (is_frist) {
    dest_buff[9] = ((0x0E&(video_pts>>29))|0x31);
    dest_buff[10] = ((video_pts>>22)&(0xFF));
    dest_buff[11] = (((video_pts>>14)&(0xFE))|0x01);
    dest_buff[12] = ((video_pts>>7)&(0xFF));
    dest_buff[13] = (((video_pts<<1)&0xFE)|0x01);	
  }
  else {
    dest_buff[9]  = 0x21;
    dest_buff[10] = 0x00;
    dest_buff[11] = 0x01;
    dest_buff[12] = 0x00;
    dest_buff[13] = 0x01;
  }
  
  temp_len = 14 ;
  
  memcpy(dest_buff + temp_len , src_buff , src_len);
  dest_len = temp_len + src_len;

  return kReturnOk;
  
}
//拆分NAL包
int PsPacketBuilder::GeneratePacketsFromFrame(const uint8_t* src_buff, int src_len, uint8_t* dest_buff, int& dest_len)
{
  //DCHECK(src_buff);
  //DCHECK(dest_buff);
  int old_index = 0;
  int packet_count = 0;
  int total_len = 0; 
  int packet_nal_len = 0;
  
  while (src_len-old_index > 60*1024) {
    if (packet_count == 0) {
      PacketVideoFrame(src_buff+old_index, 60*1024, dest_buff+total_len, packet_nal_len);	
    }
    else {
      PacketVideoFrame(src_buff+old_index, 60*1024, dest_buff+total_len, packet_nal_len, false);
    }
    old_index += 60*1024;
    total_len += packet_nal_len;
    packet_count++;
  }

  if (packet_count == 0) {
    PacketVideoFrame(src_buff+old_index, src_len-old_index, dest_buff+total_len, packet_nal_len);	
  }
  else {
    PacketVideoFrame(src_buff+old_index, src_len-old_index, dest_buff+total_len, packet_nal_len, false);
  }	
  packet_count++;
  total_len += packet_nal_len;
  dest_len = total_len;
  return kReturnOk;
}


}  // namespace mpeg2
}  // namespace media




