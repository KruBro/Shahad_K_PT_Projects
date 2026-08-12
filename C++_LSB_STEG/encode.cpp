#include "encode.h"

std::expected<EncodeInfo, Status> EncodeInfo::create(char *argv[])
{
    // Guard 1: source image must exist and contain ".bmp"
    if (argv[2] == NULL || std::string(argv[2]).find(".bmp") == std::string::npos)
    {
        return std::unexpected(Status::e_failure);
    }
    std::string src = std::string(argv[2]);

    // Guard 2: secret file must exist and contain ".txt"
    if (argv[3] == NULL || std::string(argv[3]).find(".txt") == std::string::npos)
    {
        return std::unexpected(Status::e_failure);
    }
    std::string secret = std::string(argv[3]);

    // Extension is hardcoded ".txt" in the original C too — same here
    std::string ext = ".txt";

    // stego filename: use argv[4] if present, else default
    std::string stego = (argv[4] != NULL) ? std::string(argv[4]) : std::string("stego.bmp");


    return EncodeInfo(src, secret, ext, stego);
}

void lsb :: encode_byte_to_lsb(char data, std :: span<char, bits_per_byte> image_buffer)
{
    for(std :: size_t i = 0; i < bits_per_byte; i++)
    {
        image_buffer[i] = ((image_buffer[i] & 0xFE) | ((data >> i) & 1));
    }
}

std :: expected <void, Status> lsb :: encode_data_to_image(std :: span<const char> data, std :: ifstream& fptr_src_image, std :: ofstream& fptr_stego_image)
{
    std :: array <char, lsb :: bits_per_byte> image_buffer;
    for(std :: size_t i = 0; i < data.size(); i++)
    {
        fptr_src_image.read(image_buffer.data(), lsb :: bits_per_byte);
        if(!fptr_src_image)
        {
            return std :: unexpected(Status :: e_failure);
        }

        encode_byte_to_lsb(data[i], std :: span <char, bits_per_byte> (image_buffer));

        fptr_stego_image.write(image_buffer.data(), lsb :: bits_per_byte);

        if(!fptr_stego_image)
        {
            return std :: unexpected(Status :: e_failure);
        }
    }

    return {};
}

std :: expected <void, Status> lsb :: encode_size_to_lsb(int size, std :: ifstream& fptr_src_image, std :: ofstream& fptr_stego_image)
{
    std :: array <char, 32> buffer;

    fptr_src_image.read(buffer.data(), 32);

    if(!fptr_src_image)
    {
        return std :: unexpected(Status :: e_failure);
    }

    for(std :: size_t i = 0; i < 32; i++)
    {
        buffer[i] = ((buffer[i] & 0xFE) | ((size >> i) & 1));
    }

    fptr_stego_image.write(buffer.data(), 32);

    if(!fptr_stego_image)
    {
        return std :: unexpected(Status :: e_failure);
    }

    return {};
}

std :: size_t bmp::get_image_size_for_bmp(std :: ifstream& fptr_src_image)
{
    uint32_t width, height;
    fptr_src_image.seekg(18);
    fptr_src_image.read(reinterpret_cast<char*> (&width), sizeof(uint32_t));
    fptr_src_image.read(reinterpret_cast<char*> (&height), sizeof(uint32_t));

    return static_cast<size_t>(width) * height * 3;
}

std::size_t bmp::get_file_size(std::ifstream& fptr)
{
    fptr.seekg(0, std::ios::end);
    return static_cast<std :: size_t>(fptr.tellg());
}

std :: expected <void, Status> bmp :: copy_bmp_header(std :: ifstream& fptr_src_image, std :: ofstream& fptr_stego_image)
{
    fptr_src_image.seekg(0);
    std :: array<char, header_size> header;

    fptr_src_image.read(header.data(), header_size);
    if(!fptr_src_image)
    {
        return std :: unexpected(Status :: e_failure);
    }

    fptr_stego_image.write(header.data(), header_size);
    if(!fptr_stego_image)
    {
        return std :: unexpected(Status :: e_failure);
    }

    return {};
}

std :: expected <void, Status> bmp :: copy_remaining_img_data(std :: ifstream& fptr_src_image, std :: ofstream& fptr_stego_image)
{
    fptr_stego_image << fptr_src_image.rdbuf();

    if(!fptr_stego_image)
    {
        return std :: unexpected(Status :: e_failure);
    }

    return {};
}

bool EncodeInfo::check_capacity()
{
    this->image_capacity = bmp::get_image_size_for_bmp(this->fptr_src_image);
    this->size_secret_file = bmp::get_file_size(this->fptr_secret);

    std :: size_t required_bytes = (((magic_string.size()) + (sizeof(int)) +
    (this->extn_secret_file.size()) + (sizeof(int)) + 
    (this->size_secret_file)) * 8);

    return (this->image_capacity - bmp::header_size) > required_bytes;
}

std :: expected <void, Status> EncodeInfo :: encode_magic_string()
{
    return lsb :: encode_data_to_image(magic_string, this->fptr_src_image, this->fptr_stego_image);
}

std :: expected <void, Status> EncodeInfo::encode_secret_file_extn_size()
{
    return lsb :: encode_size_to_lsb(static_cast<int>(this->extn_secret_file.size()), this->fptr_src_image, this->fptr_stego_image);
}

std :: expected <void, Status> EncodeInfo :: encode_secret_file_extn()
{
    return lsb :: encode_data_to_image(std :: span<const char>(this->extn_secret_file.data(), this->extn_secret_file.size()), 
        this->fptr_src_image, this->fptr_stego_image);
}

std :: expected <void, Status> EncodeInfo :: encode_secret_file_size()
{
    return lsb :: encode_size_to_lsb(static_cast<int>(this->size_secret_file),
    this->fptr_src_image, this->fptr_stego_image);
}

std :: expected <void, Status> EncodeInfo :: encode_secret_file_data()
{
    std :: array <char, 1024> buffer;
    std ::size_t remaining = this->size_secret_file;

    this->fptr_secret.seekg(0);

    while(remaining > 0)
    {
        std :: size_t to_read = (remaining < 1024) ? remaining : 1024;

        this->fptr_secret.read(buffer.data(), to_read);
        if(!(this->fptr_secret))
        {
            return std :: unexpected(Status :: e_failure);
        }
        std :: size_t read_count = this->fptr_secret.gcount();

        if(!(lsb :: encode_data_to_image(std :: span<const char>((buffer.data()), read_count), 
            this->fptr_src_image, this->fptr_stego_image)))
        {
            return std :: unexpected(Status :: e_failure);
        }

        remaining -= read_count;
    }

    return {};
}

std :: expected <void, Status> EncodeInfo :: do_encoding()
{
    if(!check_capacity())
    {
        std::cout << "[FAILURE] : Image Capacity Too Small" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }

    std :: cout << "[SUCCESS] : Check Capacity"  << std :: endl;

    if(!(bmp::copy_bmp_header(fptr_src_image, fptr_stego_image)))
    {
        std :: cout << "[FAILURE] : Header File Copy Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }
    std :: cout << "[SUCCESS] : Header File Copied" << std :: endl;

    if(!(encode_magic_string()))
    {
        std :: cout << "[FAILURE] : Magic String Encoding Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }
    std :: cout << "[SUCCESS] : Magic String Encoded" << std :: endl;

    if(!(encode_secret_file_extn_size()))
    {
        std :: cout << "[FAILURE] : Secret File Extension Size Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }

    std :: cout << "[SUCCESS] : Secret File Extension Encoded" << std :: endl;

    if(!(encode_secret_file_extn()))
    {
        std :: cout << "[FAILURE] : Secret File Extension Encoding Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }
    std :: cout << "[SUCCESS] : Secret File Extension Encoded" << std :: endl;

    if(!(encode_secret_file_size()))
    {
        std :: cout << "[FAILURE] : Secret File Size Encoding Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }
    std :: cout << "[SUCCESS] : Secret File Size Encoded" << std :: endl;

    if(!(encode_secret_file_data()))
    {
        std :: cout << "[FAILURE] : Secret File Data Encoding Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }
    std :: cout << "[SUCCESS] : Secret File Data Encoded" << std :: endl;

    if(!(bmp::copy_remaining_img_data(fptr_src_image, fptr_stego_image)))
    {
        std :: cout << "[FAILURE] : Remaining Data Copy Failed" << std :: endl;
        return std :: unexpected (Status::e_failure);
    }
    std :: cout << "[SUCCES] : Remaining Data Copied" << std :: endl;


    return {};
}