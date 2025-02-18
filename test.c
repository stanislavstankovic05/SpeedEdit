#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>

void copyJPEG(const char *input_filename, const char *output_filename, int quality) {
    // 1. Open input JPEG file
    FILE *infile = fopen(input_filename, "rb");
    if (!infile) {
        perror("Error opening input file");
        return;
    }

    // 2. Initialize decompression
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    // 3. Get image properties
    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int num_components = cinfo.output_components;

    printf("Reading image: %dx%d, %d components\n", width, height, num_components);

    // 4. Allocate buffer
    unsigned char *imageData = (unsigned char *)malloc(width * height * num_components);
    if (!imageData) {
        fprintf(stderr, "Memory allocation failed\n");
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return;
    }

    // 5. Read scanlines
    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = &imageData[cinfo.output_scanline * width * num_components];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    // 6. Finish reading
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    // 7. Open output file
    FILE *outfile = fopen(output_filename, "wb");
    if (!outfile) {
        perror("Error opening output file");
        free(imageData);
        return;
    }

    // 8. Initialize compression
    struct jpeg_compress_struct out_cinfo;
    struct jpeg_error_mgr out_jerr;

    out_cinfo.err = jpeg_std_error(&out_jerr);
    jpeg_create_compress(&out_cinfo);
    jpeg_stdio_dest(&out_cinfo, outfile);

    // 9. Set image properties for output
    out_cinfo.image_width = width;
    out_cinfo.image_height = height;
    out_cinfo.input_components = num_components;
    out_cinfo.in_color_space = (num_components == 1) ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&out_cinfo);
    jpeg_set_quality(&out_cinfo, quality, TRUE);

    // 10. Start compression
    jpeg_start_compress(&out_cinfo, TRUE);

    // 11. Write scanlines
    while (out_cinfo.next_scanline < out_cinfo.image_height) {
        row_pointer[0] = &imageData[out_cinfo.next_scanline * width * num_components];
        jpeg_write_scanlines(&out_cinfo, row_pointer, 1);
    }

    // 12. Finish compression
    jpeg_finish_compress(&out_cinfo);
    jpeg_destroy_compress(&out_cinfo);
    fclose(outfile);
    free(imageData);

    printf("Image copied successfully to: %s\n", output_filename);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <input.jpg> <output.jpg>\n", argv[0]);
        return 1;
    }

    copyJPEG(argv[1], argv[2], 90); // Default quality: 90
    return 0;
}

