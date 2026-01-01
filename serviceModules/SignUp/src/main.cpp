#include "SignUp.srpc.h"
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
#include <signal.h>

using namespace std;
using namespace srpc;
using namespace std::literals;

// your db address
static std::string url = "mysql://root:username@ip/db";
static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int)
{
	wait_group.done();
}

void signUpMysqlCallback(WFMySQLTask* task, EchoResponse *response){
    int state = task->get_state();
    if(state != WFT_STATE_SUCCESS){
        response->set_code(500);
        response->set_message("MqSQL connect errot!");
        cerr << WFGlobal::get_error_string(state,task->get_error()) << endl;
        return ;
    }

    protocol::MySQLResponse* resp = task->get_resp();
    if (resp->get_packet_type() == MYSQL_PACKET_ERROR) {
        response->set_code(500);
        cerr << "error code: " << resp->get_error_code() 
             << ", error message: " << resp->get_error_msg() << endl;

        if (resp->get_error_code() == 1062) {  // ER_DUP_ENTRY
            response->set_message("username is exists!");
        } else {
            response->set_message("MySQL error");
        }
        
        return ;
    }
    protocol::MySQLResultCursor cursor { resp };
    if (cursor.get_cursor_status() == MYSQL_STATUS_OK) {
        response->set_code(100);
        response->set_message("insert success");
        cout << "id: " << cursor.get_insert_id() << endl;   // insert
        cout << "affected rows: " << cursor.get_affected_rows() << endl;
    }else{
        response->set_code(500);
    }
}

class SignUpServiceImpl : public SignUp::Service
{
public:

	void Echo(EchoRequest *request, EchoResponse *response, srpc::RPCContext *ctx) override
	{
		string username = request->username();
        string password = request->password();
        std::string salt = CryptoUtil::generate_salt();
        std::string encryptedPassword = CryptoUtil::hash_password(password,salt);
        
        WFMySQLTask* signUpMysqlTask = WFTaskFactory::create_mysql_task(url,3,bind(signUpMysqlCallback,placeholders::_1,response));
        
        std::string sql = "INSERT INTO tbl_user (username, password , salt) VALUES ('" + 
                        username + "','" + encryptedPassword + "','" + salt + "');";
        protocol::MySQLRequest* req = signUpMysqlTask->get_req();
        
        req->set_query(sql);
        cout << "[SQL]" << sql << endl;

        SeriesWork* series = ctx->get_series();
        series->push_back(signUpMysqlTask);
	}
};

int main()
{
	signal(SIGINT, sig_handler);
	unsigned short port = 12580;
	SRPCServer server;

	SignUpServiceImpl signup_impl;
	server.add_service(&signup_impl);

    if(server.start(port) == 0){
        wait_group.wait();
        server.stop();
    }
    else{
        cerr << "SRPCServer start failed!" << endl;
    }
    
	return 0;
}
