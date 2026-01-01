
#include "../inc/CryptoUtil.h"
#include "../inc/User.h"

#include "jwt.h"
#include <stdlib.h>
#include <string.h>
#include <random>
#include <stdexcept>

using namespace std;

static const char* SECRET_KEY = "$^Hk16NV"; 

string CryptoUtil::generate_salt(int length)
{
    const char* alpha = "0123456789"
                  "abcdefghijklmnopqrstuvwxyz"
                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    
    /* string result; */
    /* for (int i = 0; i < length; ++i) { */
        /* result += alpha[rand() % 62]; */
    /* } */
    std::random_device rd;
    std::uniform_int_distribution<size_t> dist(0,strlen(alpha) - 2);

    string result;
    result.reserve(length);

    for(int i = 0;i < length; ++i){
        result.push_back(alpha[dist(rd)]);
    }
    return result;
}

string CryptoUtil::hash_password(const string& password, const string& salt, const EVP_MD* md)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();		// 创建 EVP 上下文
    unsigned char hash[EVP_MAX_MD_SIZE];	// EVP_MAX_MD_SIZE: 最大哈希长度
    unsigned int hash_len;					// 用来接收实际的哈希长度

    EVP_DigestInit_ex(ctx, md, NULL);		// 初始化上下文，采用 sha256 哈希算法
    EVP_DigestUpdate(ctx, password.c_str(), password.size());	// 更新上下文
    EVP_DigestUpdate(ctx, salt.c_str(), salt.size());
    EVP_DigestFinal_ex(ctx, hash, &hash_len);		    // 计算哈希值


    char result[2 * EVP_MAX_MD_SIZE + 1] = { '\0' };
    for (unsigned int i = 0; i < hash_len; i++) {			    // 转换成十六进制字符
        sprintf(result + 2 * i, "%02x", hash[i]);
    }

    EVP_MD_CTX_free(ctx);							// 释放上下文
    return result;
}

string CryptoUtil::generate_token(const User& user, jwt_alg_t algorithm)
{
    jwt_t* jwt;
    jwt_new(&jwt);  // 创建 JWT

    jwt_set_alg(jwt, algorithm, (unsigned char*)SECRET_KEY, strlen(SECRET_KEY));

    // 设置载荷(Payload): 用户自定义数据(不能存放敏感数据，比如：password, salt)
    jwt_add_grant(jwt, "sub", "login"); 
    jwt_add_grant_int(jwt, "id", user.id);      // 用户id
    jwt_add_grant(jwt, "username", user.username.c_str());	// 用户名字
    jwt_add_grant_int(jwt, "expire", time(NULL) + 1800);   // 过期时间 (30min)
    
    char* token = jwt_encode_str(jwt);		// token长度是不确定的，100-300字节
    string result = token;
    // 释放资源
    jwt_free(jwt);
    free(token);

    return result;
}

bool CryptoUtil::verify_token(const std::string& token, User& user)
{
    jwt_t* jwt;
    int err = jwt_decode(&jwt, token.c_str(), (unsigned char*)SECRET_KEY, strlen(SECRET_KEY));
    if (err) {
        return false;
    }

    // 验证主题
    if (strcmp("login", jwt_get_grant(jwt, "sub")) != 0) {
        jwt_free(jwt);
        return false;
    }
    // 判断是否过期
    if (jwt_get_grant_int(jwt, "expire") < time(NULL)) {
        jwt_free(jwt);
        return false;
    }
    // 解析 token
    user.id = jwt_get_grant_int(jwt, "id");
    user.username = jwt_get_grant(jwt, "username");
    
    jwt_free(jwt);
    return true;
}

std::string CryptoUtil::generate_hash_by_content(const std::string& content)
{
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
        throw std::runtime_error("EVP_MD_CTX_new failed");

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;

        // 2. 初始化：这里用 SHA-256，你也可以换成 SHA-1 等
        if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1)
            throw std::runtime_error("EVP_DigestInit_ex failed");

        // 3. 输入数据
        if (!content.empty())
        {
            if (EVP_DigestUpdate(ctx, content.data(), content.size()) != 1)
                throw std::runtime_error("EVP_DigestUpdate failed");
        }

        // 4. 得到结果
        if (EVP_DigestFinal_ex(ctx, hash, &hash_len) != 1)
            throw std::runtime_error("EVP_DigestFinal_ex failed");

        EVP_MD_CTX_free(ctx);
        ctx = nullptr;

        // 5. 转成十六进制字符串
        static const char* hex = "0123456789abcdef";
        std::string result;
        result.reserve(hash_len * 2);
        for (unsigned int i = 0; i < hash_len; ++i)
        {
            unsigned char b = hash[i];
            result.push_back(hex[b >> 4]);
            result.push_back(hex[b & 0x0F]);
        }
        return result;
}
