#include "../inc/RabbitMQ.h"



RabbitMQ::RabbitMQ()
:_host(Config::host),_port(Config::port),_username(Config::username),
_password(Config::password),_vhost(Config::vhost),_uri(Config::uri)
{

}

RabbitMQ::~RabbitMQ()
{

}

void RabbitMQ::producer(const string& message)
{
    Channel::ptr_t channel = Channel::Create(_host,_port,_username,_password,_vhost);
    BasicMessage::ptr_t msg = BasicMessage::Create(message);

    channel->BasicPublish(Config::exchange,Config::routingKey,msg);
}

void RabbitMQ::consumer(int count)
{
    Channel::ptr_t channel = Channel::CreateFromUri(Config::uri);
    const string &que = "queue1";
    channel->BasicConsume(que);

    // Envelope::ptr_t envelope = channel->BasicConsumeMessage();

    int consumerCount = 0;
    while (count == -1 || consumerCount < count)
    {
        Envelope::ptr_t envelope = channel->BasicConsumeMessage();
        if(envelope && envelope->Message()){
            // cout << envelope->Message()->Body() << endl;
            string data = envelope->Message()->Body();
            json j = json::parse(data);
    
            string name = j["Name"];
            string path = j["Path"];
            cout << "Name: " << name << endl;
            cout << "Path: " << path << endl;
            ++consumerCount;
        }
    }
}
// int main()
// {
//     RabbitMQ rq;
//     int i = 4 ;


//     json j;
//     j["Name"] = "name";
//     j["Path"] = "path";
    
//     // rq.producer(j.dump(2));
//     rq.consumer();
   
//     return 0;
// }

