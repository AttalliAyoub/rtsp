#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

using namespace std;

bool load_frame(const char* filePath, int* width_out, int* height_out, uint8_t** data_out);