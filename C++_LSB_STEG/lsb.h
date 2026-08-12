#ifndef LSB_H
#define LSB_H

#include <span>
#include <array>
#include <fstream>
#include <expected>
#include "types.h"

namespace lsb {
    constexpr std::size_t bits_per_byte = 8;

    void encode_byte_to_lsb(char data, std::span<char, bits_per_byte> image_buffer);
    std::expected<void, Status> encode_data_to_image(std::span<const char> data, std::ifstream& fptr_src_image, std::ofstream& fptr_stego_image);
    std::expected<void, Status> encode_size_to_lsb(int size, std::ifstream& fptr_src_image, std::ofstream& fptr_stego_image);
    std::expected<void, Status> decode_magic_string(std::ifstream& fptr_stego_image);

    void decode_lsb_to_byte(char& decode_data, std::span<const char, bits_per_byte> image_buffer);
    std :: expected<std :: size_t, Status> decode_lsb_to_size(std :: ifstream& fptr_stego_image);
    std :: expected<void, Status> decode_data_from_image(std :: span <char> data, std :: ifstream& fptr_stego_image);
    std::expected<void, Status> decode_magic_string(std::ifstream& fptr_stego_image);
}

#endif