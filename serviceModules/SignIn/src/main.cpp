#include "SignIn.srpc.h"
#include "workflow/WFFacilities.h"
#include <wfrest/HttpServer.h>
#include "workflow/WFTaskFactory.h"
#include <wfrest/PathUtil.h>
#include <workflow/MySQLResult.h>
#include <functional>
#include <iostream>
#include <thread>
#include <chrono>

#include "../../../inc/CryptoUtil.h"
#include "nlohmann/json.hpp"
#include <signal.h>

using namespace srpc;
using namespace std;

// your db address
static std::string url = "mysql://root:username@ip/db";
static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int)
{
	wait_group.done();
}

void signInMysqlCallback(WFMySQLTask* task, SignInEchoResponse *response,User* user)
{
	int state = task->get_state();
    if(WFT_STATE_SUCCESS != state){
		response->set_code(500);
		response->set_message("MySQL connect error!");
		delete user;
        return ;
    }

    protocol::MySQLResponse* mysql_resp = task->get_resp();
    if(mysql_resp->get_packet_type() == MYSQL_PACKET_ERROR){
        response->set_code(500);
		response->set_message("MySQL error!");
		delete user;
        return ;
    }

    protocol::MySQLResultCursor cursor { mysql_resp } ;
    if(cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT){
		vector<vector<protocol::MySQLCell>> matrix;
		cursor.fetch_all(matrix);
		user->id = matrix[0][0].as_int();
		user->hashcode = matrix[0][1].as_string();
		user->salt = matrix[0][2].as_string();

		std::cout << user->id << " : " << user->hashcode << " : " << user->salt << " : " << user->username << std::endl;

		if(user->hashcode == CryptoUtil::hash_password(user->password,user->salt)){
			std::string token = CryptoUtil::generate_token(*user);
			nlohmann::json msg;
			nlohmann::json data;
			data["Token"] = token;
			data["Username"] = user->username;
			data["Location"] = "/static/view/home.html";
			msg["data"] = data;

			response->set_code(100);
			response->set_message(msg.dump());
		}
		else{
			response->set_code(500);
			response->set_message("Login failed!");
		}
	}
	else{
		response->set_code(500);
		response->set_message("Username isnt exist!");
	}
	delete user;
}

class SignInServiceImpl : public SignIn::Service
{
public:

	void echoSignIn(SignInEchoRequest *request, SignInEchoResponse *response, srpc::RPCContext *ctx) override
	{
		User* user = new User;
		user->username = request->username();
        user->password = request->password();

		WFMySQLTask* signInTask = WFTaskFactory::create_mysql_task(url,3,bind(signInMysqlCallback,placeholders::_1,response,user));
		{
			protocol::MySQLRequest* mysqlReq = signInTask->get_req();
			string sql = "SELECT id,password,salt FROM tbl_user WHERE username='" + user->username + "';";
		#ifdef DEBUG
			std::cout << "[SQL]" << sql << std::endl;
		#endif
			mysqlReq->set_query(sql);
		}

		SeriesWork* series = ctx->get_series();
		series->push_back(signInTask); 
	}
};

int main()
{
	signal(SIGINT, sig_handler);
	unsigned short port = 12581;
	SRPCServer server;

	SignInServiceImpl signin_impl;
	server.add_service(&signin_impl);

	if(server.start(port) == 0){
        wait_group.wait();
        server.stop();
    }
    else{
        cerr << "sign in SRPCServer start failed!" << endl;
    }

	return 0;
}
