#ifndef DECODE_H
#define DECODE_H

#include <iostream>
#include <string>
#include <fstream>
#include <expected>
#include <span>
#include <array>
#include <cstdint>
#include "types.h"
#include "common.h"
#include "lsb.h"
#include "bmp.h"

class DecodeInfo {
private:
    std::string stego_image_fname;
    std::ifstream fptr_stego_image;

    std::string secret_fname;
    std::ofstream fptr_secret;
    std::string extn_secret_file;
    size_t size_secret_file;
    size_t size_extn_size;

    DecodeInfo(std::string stego, std::string secret) :
        stego_image_fname(stego), fptr_stego_image(stego),
        secret_fname(secret), fptr_secret(secret),
        size_secret_file(0), size_extn_size(0)
    {
        ;
    }

public:
    static std::expected<DecodeInfo, Status> create(char *argv[]);
    std::expected<void, Status> decode_secret_file_extn_size();
    std::expected<void, Status> decode_secret_file_extn();
    std::expected<void, Status> decode_secret_file_size();
    std::expected<void, Status> decode_secret_file_data();
    std::expected<void, Status> do_decoding();
};

#endif