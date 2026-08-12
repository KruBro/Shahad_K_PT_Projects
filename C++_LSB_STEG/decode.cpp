#include "decode.h"

std :: expected <DecodeInfo, Status> DecodeInfo :: create(char *argv[])
{
    if(argv[2] == NULL || std::string(argv[2]).find(".bmp") == std::string::npos)
    {
        return std :: unexpected(Status :: e_failure);
    }

    std :: string output = (argv[3] != NULL) ? std :: string(argv[3]) : "output.txt";
    return DecodeInfo(std :: string(argv[2]), output);
}

std::expected<void, Status> DecodeInfo::decode_secret_file_extn_size()
{
    auto result = lsb::decode_lsb_to_size(fptr_stego_image);
    if (!result)
    {
        return std::unexpected(Status::e_failure);
    }

    size_extn_size = result.value();
    return {};
}

std :: expected<void, Status> DecodeInfo::decode_secret_file_size()
{
    auto result = lsb::decode_lsb_to_size(fptr_stego_image);
    if(!result)
    {
        return std :: unexpected(Status::e_failure);
    }

    size_secret_file = result.value();
    return {};
}

std :: expected<void, Status> DecodeInfo::decode_secret_file_extn()
{
    extn_secret_file.resize(size_extn_size);
    auto result = lsb::decode_data_from_image(std::span<char>(extn_secret_file.data(), extn_secret_file.size()), fptr_stego_image);
    if(!result)
    {
        return std :: unexpected(Status::e_failure);
    }

    return{};
}

std::expected<void, Status> DecodeInfo::decode_secret_file_data()
{
    char ch_data;
    std::array<char, 8> image_buffer;

    for (std::size_t i = 0; i < size_secret_file; i++)
    {
        fptr_stego_image.read(image_buffer.data(), 8);
        if (!fptr_stego_image)
        {
            return std::unexpected(Status::e_failure);
        }

        lsb::decode_lsb_to_byte(ch_data, std::span<const char, lsb::bits_per_byte>(image_buffer));

        fptr_secret.write(&ch_data, 1);
        if (!fptr_secret)
        {
            return std::unexpected(Status::e_failure);
        }
    }

    return {};
}



void lsb :: decode_lsb_to_byte(char& decode_data, std::span<const char, bits_per_byte> image_buffer)
{
    decode_data = 0;
    for(std :: size_t i = 0; i < bits_per_byte; i++)
    {
        decode_data |= (image_buffer[i] & 1) << i;
    }
}

std :: expected<std :: size_t, Status> lsb :: decode_lsb_to_size(std :: ifstream& fptr_stego_image)
{
    std :: array <char, 32> image_buffer;

    fptr_stego_image.read(image_buffer.data(), 32);
    if(!fptr_stego_image)
    {
        return std :: unexpected(Status::e_failure);
    }

    size_t size  = 0;
    for(int i = 0; i < 32; i++)
    {
        size |= (image_buffer[i] & 1) << i;
    }

    return size;
}

std :: expected<void, Status> lsb :: decode_data_from_image(std :: span <char> data, std :: ifstream& fptr_stego_image)
{
    std :: array <char, 8> image_buffer;

    for(size_t i = 0; i < data.size(); i++)
    {
        fptr_stego_image.read(image_buffer.data(), 8);
        if(!fptr_stego_image)
        {
            return std :: unexpected(Status::e_failure);
        }

        decode_lsb_to_byte(data[i], std::span<const char, lsb::bits_per_byte>(image_buffer));
    }

    return {};
}

std::expected<void, Status> lsb::decode_magic_string(std::ifstream& fptr_stego_image)
{
    std::array<char, magic_string.size()> decoded_buffer;

    auto read_result = decode_data_from_image(std::span<char>(decoded_buffer), fptr_stego_image);
    if (!read_result)
    {
        return std::unexpected(Status::e_failure);
    }

    std::string_view decoded_view(decoded_buffer.data(), decoded_buffer.size());
    if (decoded_view != magic_string)
    {
        return std::unexpected(Status::e_failure);
    }

    return {};
}

std::expected<void, Status> DecodeInfo::do_decoding()
{
    fptr_stego_image.seekg(bmp::header_size);
    if (!fptr_stego_image)
    {
        std::cout << "[FAILURE] : Seeking Past Header Failed" << std::endl;
        return std::unexpected(Status::e_failure);
    }
    std::cout << "[SUCCESS] : Header Skipped" << std::endl;

    if (!lsb::decode_magic_string(fptr_stego_image))
    {
        std::cout << "[FAILURE] : Magic String Mismatch" << std::endl;
        return std::unexpected(Status::e_failure);
    }
    std::cout << "[SUCCESS] : Magic String Verified" << std::endl;

    if (!decode_secret_file_extn_size())
    {
        std::cout << "[FAILURE] : Extension Size Decoding Failed" << std::endl;
        return std::unexpected(Status::e_failure);
    }
    std::cout << "[SUCCESS] : Extension Size Decoded" << std::endl;

    if (!decode_secret_file_extn())
    {
        std::cout << "[FAILURE] : Extension Decoding Failed" << std::endl;
        return std::unexpected(Status::e_failure);
    }
    std::cout << "[SUCCESS] : Extension Decoded" << std::endl;

    if (!decode_secret_file_size())
    {
        std::cout << "[FAILURE] : Secret File Size Decoding Failed" << std::endl;
        return std::unexpected(Status::e_failure);
    }
    std::cout << "[SUCCESS] : Secret File Size Decoded" << std::endl;

    if (!decode_secret_file_data())
    {
        std::cout << "[FAILURE] : Secret File Data Decoding Failed" << std::endl;
        return std::unexpected(Status::e_failure);
    }
    std::cout << "[SUCCESS] : Secret File Data Decoded" << std::endl;

    return {};
}