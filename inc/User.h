#pragma once

#include <string>

struct User
{
    int id;
    std::string username;
    std::string password;
    std::string hashcode;
    std::string salt;
    std::string createdAt;
    // ...
};