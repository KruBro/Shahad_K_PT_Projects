#ifndef BMP_H
#define BMP_H

#include <fstream>
#include <expected>
#include "types.h"

namespace bmp {
    constexpr std::size_t header_size = 54;

    std::size_t get_image_size_for_bmp(std::ifstream& fptr_src_image);
    std::size_t get_file_size(std::ifstream& fptr);
    std::expected<void, Status> copy_bmp_header(std::ifstream& fptr_src_image, std::ofstream& fptr_stego_image);
    std::expected<void, Status> copy_remaining_img_data(std::ifstream& fptr_src_image, std::ofstream& fptr_stego_image);
}

#endif