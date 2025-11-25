#include "image.hh"

#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>

// Bytes per pixel, 24b rgb default
#define BPP 3

// Result allocated with the reurner now owning it!
size_t CreateImage(u16 Width, u16 Height, u8* Data, u8*& OutputData) {
    u8* result = nullptr;
    size_t result_size = 0;

    jpeg_error_mgr err;
    jpeg_compress_struct cinfo;
    jpeg_create_compress(&cinfo);
    cinfo.err = jpeg_std_error(&err);

    jpeg_mem_dest(&cinfo, &result, &result_size);

    cinfo.image_width = Width;
    cinfo.image_height = Height;
    cinfo.input_components = BPP;
    cinfo.jpeg_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, true);

    jpeg_start_compress(&cinfo, true);

    while(cinfo.next_scanline < cinfo.image_height) {
        u_char* line = Data + (cinfo.next_scanline * cinfo.image_width * BPP);
        jpeg_write_scanlines(&cinfo, &line, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    OutputData = result;
    return result_size;
}
