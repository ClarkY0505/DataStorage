#include "UpLoad.srpc.h"
#include "workflow/WFFacilities.h"

using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

class UpLoadServiceImpl : public UpLoad::Service
{
public:

	void Echo(UpLoadEchoRequest *request, UpLoadEchoResponse *response, srpc::RPCContext *ctx) override
	{
		// TODO: fill server logic here
	}
};

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	unsigned short port = 1412;
	SRPCServer server;

	UpLoadServiceImpl upload_impl;
	server.add_service(&upload_impl);

	server.start(port);
	wait_group.wait();
	server.stop();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
