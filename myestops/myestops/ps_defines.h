#ifndef MEDIA_CAST_FORMATS_MPEG2_PS_DEFINES_H_
#define MEDIA_CAST_FORMATS_MPEG2_PS_DEFINES_H_

#include <stdint.h>

namespace media {
namespace mpeg2 {

enum ReturnCode {
  kReturnOk = 0,
  kErrorParam,
  kErrorStream,
  kErrorFormat,
  kErrorH264,
  kErrorPesLen,
};

#define CHECK_PS_HEADER(X)  if( X[0] != (char)0x00 || X[1] != (char)0x00 || X[2] != (char)0x01 || X[3] != (char)0xBA ) \
						{\
							return PS_Error_Stream;\
						}\

#define SAFE_DELETE_ARRAY(X)  if( NULL != X) { delete[] X ; X = NULL;}

//方法
#define OFFSET(x)  (90000/x)
#define UNITS_IN_TICKET(x)  (90000/(x*2))

static const uint32_t kPSPacketStartCodeLen = 4;
static const uint32_t kPSHeaderLen = 14;
static const uint32_t kPSHeaderLenAligned = 16;

static const uint32_t kMaxPsbufferSize = 614400;
static const uint32_t kMuxRate = 6150;
static const uint32_t kMaxVideoLen = 1024*1024;

static const uint8_t kPSHeader[kPSPacketStartCodeLen] = {0x00 , 0x00 , 0x01 , 0xBA};
static const uint8_t kPSSystemHeader[kPSPacketStartCodeLen] = {0x00 , 0x00 , 0x01 , 0xBB};
static const uint8_t kPSMapHeader[kPSPacketStartCodeLen] = {0x00 , 0x00 , 0x01 , 0xBC};
static const uint8_t kPSVideoPesHeader[kPSPacketStartCodeLen] = {0x00 , 0x00 , 0x01 , 0xE0};



// Ps common Header
static const uint8_t kPsStreamIdsBitMask = 0xFF;
static const uint32_t kPsConstantStartCode = 0x000001;
static const uint8_t kPsCommonHeaderLength = 6;

static const uint8_t kAudioStreamId = 0x0C;

struct PsPacket {
  uint8_t out_buf[2*kMaxVideoLen];
  int  len;
};

struct PsCommonHeader {
  PsCommonHeader();
  // 从通用头中提取的固定字段
  uint32_t packet_start_code_prefix;
  uint8_t stream_id;
  uint16_t content_length;
};

}  // namespace mpeg2
}  // namespace mpeg2
#endif  // MEDIA_CAST_FORMATS_MPEG2_PS_DEFINES_H_