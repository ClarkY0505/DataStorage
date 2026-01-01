#pragma once

#include <string>
class Config
{
public:
    static std::string AccessKeyId;
    static std::string AccessKeySecret;
    static std::string Endpoint;
    static std::string Bucket;

    static std::string host;  // RabbitMQ Server的地址  
    static int port;
    static std::string username;  
    static std::string password;  
    static std::string vhost;  
    static std::string uri;

    static std::string exchange;	// 指定交换机
    static std::string routingKey;
};

