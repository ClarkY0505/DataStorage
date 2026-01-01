#include "../inc/OssCustom.h"

#include <filesystem>

int64_t getFileSize(const std::string& file)
{
    std::fstream f(file, std::ios::in | std::ios::binary);
    f.seekg(0, f.end);
    int64_t size = f.tellg();
    f.close();
    return size;
}

std::string GetMimeType(const std::string& extension) {
    static const std::unordered_map<std::string, std::string> mimeMap = {
        {"png", "image/png"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"gif", "image/gif"},
        {"txt", "text/plain"},
        {"html", "text/html"},
        {"pdf", "application/pdf"},
        {"zip", "application/zip"}
    };
    auto it = mimeMap.find(extension);
    if (it != mimeMap.end()) {
        return it->second;
    }

    return "application/octet-stream";
}

OssCustom::CustomCredentialsProvider::CustomCredentialsProvider()
{

}

OssCustom::CustomCredentialsProvider::~CustomCredentialsProvider()
{

}

Credentials OssCustom::CustomCredentialsProvider::getCredentials()
{    
    auto cred = Credentials(Config::AccessKeyId,Config::AccessKeySecret,"");
    return cred;
}

OssCustom::ObjectFunction& OssCustom::ObjectFunction::getInstance()
{
    static ObjectFunction instance;
    return instance;
}

OssCustom::ObjectFunction::ObjectFunction()
:_bucket(Config::Bucket)
{
    InitializeSdk();
    ClientConfiguration conf;
    auto credentialsProvider = std::make_shared<CustomCredentialsProvider>();
    _client = new OssClient(Config::Endpoint,credentialsProvider,conf);
    // CreateBucketRequest req(_bucket);
    // _client->CreateBucket(req);
    std::string Region = "cn-wuhan";
    _client->SetRegion(Region);
}

OssCustom::ObjectFunction::~ObjectFunction(){
    if(_client){
        delete _client;
        _client = nullptr;
    }
    ShutdownSdk();
}

void OssCustom::ObjectFunction::PrintError(const std::string &funcName, const OssError &error){
    std::cout << funcName << " fail" <<
        ",code:" << error.Code() <<
        ",message:" << error.Message() <<
        ",request_id:" << error.RequestId() << std::endl;
}

void OssCustom::ObjectFunction::putFolder(const std::string &dirObjectName,const std::string &path)
{
    // InitializeSdk();
    // std::string BucketName = "mytestproject";
    // std::string DirName = dirName;
    auto content = std::make_shared<std::stringstream>();
    PutObjectRequest req(_bucket,dirObjectName,content);

    auto outcome = _client->PutObject(req);
    if(!outcome.isSuccess()){
        PrintError(__FUNCTION__,outcome.error());
    }

    std::cout << __FUNCTION__ << " success, dir : " << dirObjectName << std::endl;

    std::string subDirName = dirObjectName + "/";
    for(const auto &entry : std::filesystem::directory_iterator(path)){
        std::filesystem::path relativePath = std::filesystem::relative(entry.path(), dirObjectName);
        std::string ossObjectKey = dirObjectName + relativePath.generic_string();
        std::cout << ossObjectKey << " : " << entry.path().string() << std::endl;
        
        std::string tmpStr = entry.path().string();
        // std::cout << "[Str]"<< tmpStr << std::endl;

        std::string prefix = "/";
        std::string::size_type pos = tmpStr.find_last_of(prefix);
        if(std::string::npos == pos){
            std::cerr << "Not Found Username" << std::endl;
            return ;
        }
        std::string fileObjectName = tmpStr.substr(pos + 1);
        // std::cout << "[File]" << fileObjectName << std::endl;

        if(entry.is_directory()){
            std::cout << "[Dir]" << ossObjectKey + "/" + fileObjectName << std::endl;
            putFolder(ossObjectKey  + fileObjectName + "/", entry.path().string());
        }
        else if(entry.is_regular_file()){
            putObjectFromBuffer(ossObjectKey + fileObjectName, entry.path().string());
        }
    }
    // ShutdownSdk(); 
}

void OssCustom::ObjectFunction::putObjectFromBuffer(const std::string &objectName,const std::string &path)
{
    // fecth file size
    std::ifstream file(path,std::ios::binary | std::ios::ate);
    if(!file){
        std::cerr << "Cannot open file : " << path << std::endl;
        return ;
    }

    std::streamsize fileSize = file.tellg();
    file.seekg(0,std::ios::beg);
    
    // storage allocation
    std::string fileContent;
    fileContent.resize(fileSize);

    // read the file into memory
    if(!file.read(&fileContent[0],fileSize)){
        std::cerr << "Failed to read file: " << path << std::endl;
        return ;
    }

    // std::cout << fileSize << std::endl;
    file.close();

    /* 在一开始我想构建一个自定义的内存缓冲流（MemoryStreamBuffer）
    *  用来转换图片二进制
    *  发现OSS只支持传入shared_ptr<iostream>
    *  但是将我自定义的MemoryStreamBuffer转换为shared_ptr<iostream>
    *  目前来说可能是一个大工程
    *  
    *  这里直接选择将string类型转换为 stringstream
    *  通过二进制读取存入到stringstream中
    */
    auto content = std::make_shared<std::stringstream>(fileContent);
    content->seekg(0,std::ios::beg);

    // 转译图片后缀为OSS能识别的类型如："png", "image/png"
    std::string extension = path.substr(path.find_last_of(".") + 1);
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    //std::cout << extension << std::endl;
    PutObjectRequest req(_bucket,objectName,content);
    req.MetaData().setContentType(GetMimeType(extension));
    // std::cout << GetMimeType(extension) << std::endl;

    auto outcome = _client->PutObject(req);

    if(!outcome.isSuccess()){
        PrintError(__FUNCTION__, outcome.error());
        return ;
    }

    std::cout << __FUNCTION__ << " success, ETag:" << outcome.result().ETag() << std::endl;
}

void OssCustom::ObjectFunction::putObjectFromFile(const std::string &objectName,const std::string &path){
    auto outcome = _client->PutObject(_bucket,objectName,path);
    if(!outcome.isSuccess()){
        PrintError(__FUNCTION__,outcome.error());
        return ;
    }

    std::cout << __FUNCTION__ << " success, ETag:" << outcome.result().ETag() << std::endl;
}

OssClient* OssCustom::ObjectFunction::getClient(){
    return _client;
}

// int main(void){
//     // InitializeSdk();
//     // OssCustom::ObjectFunction object;
//     // object.putFolder("dir3/");
//     // object.putObjectFromBuffer("dir1/b.txt","../tmp/b.txt");
//     // object.putObjectFromFile("dir1/d.txt","../tmp/c.txt");
//     // ShutdownSdk();
//     auto& oss = OssCustom::ObjectFunction::getInstance();
//     // oss.putObjectFromFile("dir2/","../tmp/collector.txt");
//     oss.putFolder("tmp/","../tmp/");
// }