#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ps_packet_builder.h"

/* 帧类型定义*/
enum PackTypeFlag
{
	PACK_TYPE_FRAME_P = 0,
	PACK_TYPE_FRAME_I,
	PACK_TYPE_FRAME_B,
	PACK_TYPE_FRAME_AUDIO,
	PACK_TYPE_FRAME_JEPG,
	PACK_TYPE_FRAME_MOTION,
	PACK_TYPE_FRAME_NUM,
	PACK_TYPE_FRAME_NULL = 0xff,/*结束，无数据*/
};


struct h264FRAME_INFO
{
	int framelen;
	int frametype;
};

int main()
{

	struct h264FRAME_INFO frameinfo;

	FILE* fp_h264 = NULL;
	int ret = 0;

	uint8_t* pBuf = new unsigned char[1024 * 1024];
	fp_h264 = fopen("video.es", "rb");
	FILE* fp_index = fopen("len_type.type", "rb");
	FILE* fp_ps = fopen("video.ps", "wb+");
	int firstI = 0;
	media::mpeg2::PsPacketBuilder builder;
	int framecount = 0;
	while (1)
	{
		memset(pBuf, 0, 1024 * 1024);
		ret = fread(&frameinfo, sizeof(frameinfo), 1, fp_index);

		if (ret <= 0)
		{
			printf("read frame index over, total:%d\n", framecount);
			break;
		}
		
		framecount++;
		ret = fread(pBuf, 1, frameinfo.framelen, fp_h264);
		media::mpeg2::PsPacket* packet = new media::mpeg2::PsPacket();
		memset(packet->out_buf, 0, sizeof(packet->out_buf));
		packet->len = 0;
		
		if (frameinfo.frametype == 0) {
			if (firstI == 1) {
				builder.PacketPFrame(pBuf, frameinfo.framelen, packet);
			}
		}
		else {
			builder.PacketIFrame(pBuf, frameinfo.framelen, packet, 25, 7);
			firstI = 1;
		}

		if (packet->len != 0) {
			printf("%d\n", packet->len);
			int wlen = fwrite(packet->out_buf, 1, packet->len, fp_ps);
		}
		delete packet;
		packet = NULL;
	}

	fclose(fp_h264);
	fclose(fp_index);
	fclose(fp_ps);
	delete[] pBuf;
	system("pause");
	return 0;
}