#include "encode.h"
#include "decode.h"
#include "common.h"
#include "types.h"
#include "lsb.h"
#include "bmp.h"

OperationType check_operation_type(char *argv[])
{
    if(std :: string(argv[1]) == "-e")
    {
        return OperationType::e_encode;
    }
    else if(std :: string(argv[1]) == "-d")
    {
        return OperationType::e_decode;
    }

    return OperationType::e_unsupported;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std :: cout << "[ERROR] : Missing Arguments" << std :: endl;
        std :: cout << "[USAGE] : Encoding -e <source_file_name.bmp> <secret_file_name.txt> --[stego_output.bmp]" << std :: endl;
        std :: cout << "[USAGE] : Decoding -d <stego_image_name.bmp>  --[secret_file_name.txt]" << std :: endl;
        return 1;
    }

    OperationType res = check_operation_type(argv);

    if(res == OperationType::e_encode)
    {
        auto result = EncodeInfo::create(argv).and_then([](EncodeInfo enc)
        {
            return enc.do_encoding();
        }); 
        
        if(!result)
        {
            std::cout << "[FAILURE] : Encoding Failed" << std::endl;
            return 1;
        }

        std::cout << "[SUCCESS] : Encoding Completed" << std::endl;
    }
    else if(res == OperationType::e_decode)
    {
        auto result = DecodeInfo::create(argv).and_then([](DecodeInfo dec)
        {
            return dec.do_decoding();
        }); 
        
        if(!result)
        {
            std::cout << "[FAILURE] : Decoding Failed" << std::endl;
            return 1;
        }

        std::cout << "[SUCCESS] : Decoding Completed" << std::endl;
    }
    else
    {
        std::cout << "[ERROR] : Unsupported Operation" << std::endl;
        return 1;
    }

    return 0;
}