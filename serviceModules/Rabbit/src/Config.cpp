#include <string>
#include "../inc/Config.h"

// ali-oss config
// Please configure the AccessKey or Token for the OSS.
// such as AccessKeyId,AccessKeySecret,Endpoint,Bucket


std::string Config::host = "127.0.0.1";  // RabbitMQ Server的地址  
int Config::port = 5672;            // AMQP协议
std::string Config::username = "guest";  
std::string Config::password = "guest";  
std::string Config::vhost = "/";  
std::string Config::uri = "amqp://guest:guest@localhost:5672/%2f";

std::string Config::exchange = "oss.direct";	// 指定交换机
std::string Config::routingKey = "oss";

