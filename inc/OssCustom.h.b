#pragma once 

#include <iostream>
#include <memory>
#include <sstream>
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <string>

#include <alibabacloud/oss/OssClient.h>

#include "../inc/Config.h"

using namespace AlibabaCloud::OSS;

namespace OssCustom{
    class CustomCredentialsProvider
    : public CredentialsProvider
    {
    public:
        CustomCredentialsProvider();
        ~CustomCredentialsProvider();
        Credentials getCredentials() override;
    };

    class MemoryStreamBuffer
    :public std::streambuf
    {
    public:
        MemoryStreamBuffer(const char* data,size_t size)
        :_buf(data,data + size)
        {
            char* base = _buf.data();
            setg(base,base,base + size);
            setp(base,base + size);
        }

        static std::shared_ptr<MemoryStreamBuffer> Create(const char* data, size_t size)
        {
            return std::make_shared<MemoryStreamBuffer>(data, size);
        }

    private:
        std::vector<char> _buf;
    };

    class ObjectFunction
    {
    private:
        void PrintError(const std::string &funcName, const OssError &error); 
    
    public:
        ObjectFunction();
        ~ObjectFunction();

        void putFolder(const std::string &dirName);
        void putObjectFromFile(const std::string &objectName,const std::string &path);
        void putObjectFromBuffer(const std::string &objectName,const std::string &path);

    
    private:
        OssClient *_client;
        std::string _bucket;
    };
}