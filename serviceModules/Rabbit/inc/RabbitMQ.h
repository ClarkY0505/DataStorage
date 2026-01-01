#pragma once

#include <iostream>
#include <string>

#include "nlohmann/json.hpp"
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include "Config.h"


using namespace std;
using namespace AmqpClient;
using json = nlohmann::json;

class RabbitMQ{
public:
    RabbitMQ();
    ~RabbitMQ();

    void producer(const string& message);
    void consumer(int count = -1);
    
private:
    string _host;  // RabbitMQ Server的地址  
    int _port;            // AMQP协议
    string _username;  
    string _password;  
    string _vhost;
    string _uri;      
};
