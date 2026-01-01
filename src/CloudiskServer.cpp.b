#include "../inc/CloudiskServer.h"

#include "../inc/SignUp.srpc.h"
#include "../inc/SignIn.srpc.h"

using namespace srpc;

using namespace wfrest;

// static int64_t getFileSize(const std::string& file)
// {
//     std::fstream f(file, std::ios::in | std::ios::binary);
//     f.seekg(0, f.end);
//     int64_t size = f.tellg();
//     f.close();
//     return size;
// }

static std::string url = "mysql://root:y@192.168.153.131/test";
void CloudiskServer::register_modules()
{
    register_static_resources_module();
    register_signup_module();
	register_signin_module();
    register_userinfo_module();
    register_fileupload_module();
    register_filelist_module();
    register_filedownload_module();
}

void CloudiskServer::register_static_resources_module()
{
    m_server.GET("/user/signup", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/signup.html");
    });

    m_server.GET("/static/view/signin.html", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/signin.html");
    });

    m_server.GET("/static/view/home.html", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/home.html");
    });

    m_server.GET("/static/js/auth.js", [](const HttpReq *, HttpResp * resp){
        resp->File("static/js/auth.js");
    });

    m_server.GET("/static/img/avatar.jpeg", [](const HttpReq *, HttpResp * resp){
        resp->File("static/img/avatar.jpeg");
    });

    m_server.GET("/file/upload", [](const HttpReq *, HttpResp * resp){
        resp->File("static/view/index.html");
    });

    m_server.Static("/file/upload_files","static/view/upload_files");
}

static void signUpEchoCallback(HttpResp * httpResp, EchoResponse *echoResponse, srpc::RPCContext *context){
    if(echoResponse->code() != 100){
        cout << echoResponse->message() << endl;
        httpResp->set_status(HttpStatusBadRequest);
        httpResp->String("sign up failed,maybe username is exists!");
    }
    cout << echoResponse->message() << endl;
    httpResp->String("SUCCESS");
}

void signUpCallback(const HttpReq * req, HttpResp * resp, SeriesWork* series)
{
    // fecth username and password ,which is core issue
    if(req->content_type() != APPLICATION_URLENCODED){
        // std::cout << "i am signup callback error" << std::endl;
        resp->set_status(HttpStatusBadRequest);
        return ; 
    }
    
    std::map<std::string,std::string> userDataForm = req->form_kv();
    std::string username = userDataForm["username"];
    std::string password = userDataForm["password"] ;

#ifdef DEBUG
    std::cout << "username : " << username << "\npassword : " << password << std::endl;
#endif

    const char *ip = "127.0.0.1";
    unsigned short port = 12580;
    SignUp::SRPCClient client{ ip,port };

    EchoRequest echoReq;
    echoReq.set_username(username);
    echoReq.set_password(password);
    
    SRPCClientTask* RPCtask = client.create_Echo_task(bind(signUpEchoCallback,resp,placeholders::_1,placeholders::_2));

    RPCtask->serialize_input(&echoReq);
    series->push_back(RPCtask);
}

void CloudiskServer::register_signup_module()
{
    m_server.POST("/user/signup", signUpCallback);
}

static void signInEchoCallback(HttpResp * httpResp, SignInEchoResponse *echoResponse, srpc::RPCContext *context){
    if(echoResponse->code() != 100){
        httpResp->set_status_code(to_string(echoResponse->code()));
        httpResp->append_output_body_nocopy(echoResponse->message());
        return ;   
    }
    httpResp->String(echoResponse->message());
}

void signInCallback(const HttpReq * req, HttpResp * resp, SeriesWork* series){
    if(req->content_type() != APPLICATION_URLENCODED){
        resp->set_status(HttpStatusBadRequest);
        return ;
    }

    const void* body;
    size_t size;
    req->get_parsed_body(&body,&size);
    std::string bodyInfo { static_cast<const char*>(body),size };
    // std::cout << bodyInfo << std::endl;

    std::string prefix = "&";
    std::string::size_type pos = bodyInfo.find(prefix);
    if(std::string::npos == pos){
        std::cerr << "Not Found Username" << std::endl;
        return ;
    }
    std::string username;
    username = bodyInfo.substr(0,pos);
    // username == username=yyyy
    prefix = "username=";
    pos = username.find(prefix);
    username = username.substr(pos + prefix.size());
    // std::cout << "username : " << username << std::endl;

    prefix = "password=";
    pos = bodyInfo.find(prefix);
    if(std::string::npos == pos){
        std::cerr << "Not Found Password" << std::endl;
        return ;
    }
    std::string password;
    password = bodyInfo.substr(pos + prefix.size());
    // std::cout << "password : " << password << std::endl;

    const char *ip = "127.0.0.1";
    unsigned short port = 12581;

    SignIn::SRPCClient client{ ip,port };

    SignInEchoRequest signInEchoReq;
    signInEchoReq.set_username(username);
    signInEchoReq.set_password(password);
    
    SRPCClientTask* signRPCTask = client.create_echoSignIn_task(bind(signInEchoCallback,resp,placeholders::_1,placeholders::_2));

    signRPCTask->serialize_input(&signInEchoReq);
    series->push_back(signRPCTask);
}

void CloudiskServer::register_signin_module()
{
    m_server.POST("/user/signin", signInCallback);
}

void fetchSignupAt(protocol::MySQLResultCursor* cursor, HttpResp * resp, User *user){
    if(cursor->get_cursor_status() == MYSQL_STATUS_GET_RESULT){
        std::vector<std::vector<protocol::MySQLCell>> matrix;
        cursor->fetch_all(matrix);
        // std::cout << matrix[0][0].as_string() << std::endl;
        std::string signupAt = matrix[0][0].as_string();
        nlohmann::json msg;
        nlohmann::json data;
        data["Username"] = user->username;
        data["SignupAt"] = signupAt;
        msg["data"] = data;
        resp->String(msg.dump());
    }else{
        resp->set_status(HttpStatusBadRequest);
        resp->String("fetch signupAt failed!");
    }
    delete user;
}

void userInfoCallback(const HttpReq * req, HttpResp * resp){
    User* user = new User;
    std::string username = req->query("username");
    std::string token = req->query("token");

#ifdef DEBUG
    std::cout << username << " : " << token << std::endl;
#endif
    user->username = username;
    if(CryptoUtil::verify_token(token,*user)){
        std::string sql = "SELECT created_at FROM tbl_user WHERE username='" + username + "';";
        resp->MySQL(url,sql,bind(fetchSignupAt,std::placeholders::_1,resp,user));
    }else{
        resp->set_status(HttpStatusUnauthorized);
        resp->String("Token invalid!");
    }
}

void CloudiskServer::register_userinfo_module(){
    m_server.GET("/user/info", userInfoCallback);
}

void fileUploadMysqlCallback(const std::string& username,const std::string& filename,const std::string& path,HttpResp * resp,protocol::MySQLResultCursor* cursor)
{
    if(cursor->get_cursor_status() == MYSQL_STATUS_OK && cursor->get_affected_rows() == 1){
        RabbitMQ rq;
        json j;
        j["Username"] = username;
        j["Filename"] = filename;
        j["Path"] = path;
        rq.producer(j.dump());

        resp->set_status(HttpStatusSeeOther);
        resp->set_header_pair("Location", "/static/view/home.html?message=upload_success");
    }else{
        resp->set_status(HttpStatusBadRequest);
        resp->String("sign up failed,maybe username is exists!");
    }
}

void fileUploadCallback(const HttpReq * req, HttpResp * resp){
    User* user = new User;
    std::string username = req->query("username");
    std::string token = req->query("token");
    user->username = username;

    if(CryptoUtil::verify_token(token,*user)){
        if(req->content_type() != MULTIPART_FORM_DATA){
            resp->set_status(HttpStatusBadRequest);
            delete user;
            return;
        }
        
        // map<string,pair<string,string>>
        Form& formData = req->form();
        // std::string filename = formData["file"].first;
        std::string filename = formData["file"].first;
        std::string content = formData["file"].second;

        // std::cout << filename << " : " << content << std::endl;
        filename = PathUtil::base(filename);
        // std::cout << filename << std::endl;
        mkdir("tmp",0755);
        // std::string path = "tmp/" + filename;
        std::string path = "tmp/" + username + "/";
        mkdir(path.c_str(),0755);
        path = path + filename;
        if(access(path.c_str(),F_OK) == 0){
            resp->Redirect("/static/view/home.html?err=exist",HttpStatusSeeOther);
            return ; 
        }

        std::string fileHash = CryptoUtil::generate_hash_by_content(content);
        size_t fileSize = content.size();
        resp->Save(path,std::move(content));

        std::string sql = " INSERT INTO tbl_file (uid,filename,hashcode,size) VALUES ('" + 
                            std::to_string(user->id) + "','" + filename + "','" + fileHash + 
                            "','" + std::to_string(fileSize) + "');";
#ifdef DEBUG
        std::cout << "[SQL]" << sql << std::endl; 
#endif
        resp->MySQL(url,sql,bind(fileUploadMysqlCallback,username,filename,path,resp,std::placeholders::_1));
    }
    else{
        resp->set_status(HttpStatusUnauthorized);
        resp->String("token invaild!");
    }
}

void CloudiskServer::register_fileupload_module()
{
    m_server.POST("/file/upload",fileUploadCallback);
}

void fileDownloadCallback(const HttpReq * req, HttpResp * resp){
    // TODO
    User* user = new User;
    std::string username = req->query("username");
    std::string token = req->query("token");
    std::string filename = req->query("filename");
    filename = PathUtil::base(filename);
    std::string filehash = req->query("filehash");
    user->username = username;
#ifdef DEBUG
    std::cout << username << " : " << token << " : " << filename << " : " << filehash << std::endl; 
#endif

    if(!CryptoUtil::verify_token(token,*user)){
        resp->set_status(HttpStatusUnauthorized);
        resp->String("token invaild!");
        delete user;
        return ;
    }

    std::string path = "tmp/" + filename;
#ifdef DEBUG
    std::cout << "[PATH]" << path << std::endl;
#endif
    if(access(path.c_str(),R_OK) != 0){
        resp->set_status(HttpStatusNotFound);
        resp->String("file isnt exist!");
        delete user;
        return ; 
    }
    
    resp->add_header_pair("Content-Type", "application/octet-stream");
    resp->add_header_pair("Content-Disposition","attachment; filename=\"" + filename + "\"");
    resp->File(path);
    delete user;
}

void CloudiskServer::register_filedownload_module()
{
     m_server.GET("/file/download",fileDownloadCallback);
}

void fileListMysqlCallback(HttpResp * resp,protocol::MySQLResultCursor* cursor){

    if(cursor->get_cursor_status() == MYSQL_STATUS_GET_RESULT){
        std::vector<std::vector<protocol::MySQLCell>> matrix;
        cursor->fetch_all(matrix);

        nlohmann::json msgArr;
        for(size_t i = 0 ; i < matrix.size() ; ++i){
            nlohmann::json data;
            data["FileHash"] = matrix[i][0].as_string();
            data["FileName"] = matrix[i][1].as_string();
            data["FileSize"] = matrix[i][2].as_ulonglong();
            data["UploadAt"] = matrix[i][3].as_string();
            data["LastUpdated"] = matrix[i][4].as_string();
            msgArr.push_back(data);
        }
        resp->String(msgArr.dump());
    }else{
        resp->String("Please use to your cloud disk");
    }

}

void fileListCallback(const HttpReq * req, HttpResp * resp)
{
    User* user = new User;
    std::string username = req->query("username");
    std::string token = req->query("token");
    user->username = username;

    std::string limit = req->form_kv()["limit"];

    if(CryptoUtil::verify_token(token,*user)){
        std::string sql = "select hashcode,filename,size,created_at,last_update,status from tbl_file where uid = '" + 
                           std::to_string(user->id) + "' limit " + limit + ";"; 
#ifdef DEBUG
        std::cout << "[SQL]" << sql << std::endl; 
#endif
        resp->MySQL(url,sql,bind(fileListMysqlCallback,resp,std::placeholders::_1));
    }    
}

void CloudiskServer::register_filelist_module()
{
    m_server.POST("/file/query",fileListCallback);
}

