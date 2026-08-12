#ifndef ENCODE_H
#define ENCODE_H

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

class EncodeInfo {
private:
    std::string src_image_fname;
    std::ifstream fptr_src_image;
    size_t image_capacity;

    std::string secret_fname;
    std::ifstream fptr_secret;
    std::string extn_secret_file;
    size_t size_secret_file;

    std::string stego_image_fname;
    std::ofstream fptr_stego_image;

    EncodeInfo(std::string src, std::string secret, std::string ext, std::string stego) :
    src_image_fname(src), fptr_src_image(src), image_capacity(0), secret_fname(secret), 
    fptr_secret(secret), extn_secret_file(ext), size_secret_file(0),  
    stego_image_fname(stego), fptr_stego_image(stego)
    {
        ;
    }

public:
    static std::expected <EncodeInfo, Status> create(char *argv[]);
    bool check_capacity();
    std :: expected <void, Status> encode_magic_string();
    std :: expected <void, Status> encode_secret_file_extn_size();
    std :: expected <void, Status> encode_secret_file_extn();
    std :: expected <void, Status> encode_secret_file_size();
    std :: expected <void, Status> encode_secret_file_data();
    std :: expected <void, Status> do_encoding();
};

#endif