cmake_minimum_required(VERSION 3.15)
project(CouldServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -ggdb -fno-rtti -DDEBUG -Wall -Wextra -Wpedantic")

set(Third_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/third/include")
set(Third_LIBS "${CMAKE_SOURCE_DIR}/third/include/lib/")

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${Third_INCLUDE_DIRS}
)

# ========== 主项目库 ==========
aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
add_library(${PROJECT_NAME}_static STATIC ${SRC})
add_library(${PROJECT_NAME}_shared SHARED ${SRC})

# ========== Rabbit 库 ==========
aux_source_directory(${PROJECT_SOURCE_DIR}/serviceModules/Rabbit/src RabbitSRC)
add_library(Rabbit_static STATIC ${RabbitSRC})
add_library(Rabbit_shared SHARED ${RabbitSRC})

# ========== SignUp 库 ==========
aux_source_directory(${PROJECT_SOURCE_DIR}/serviceModules/SignUp/src SignUpSRC)
add_library(SignUp_static STATIC ${SignUpSRC})
add_library(SignUp_shared SHARED ${SignUpSRC})

# ========== SignIn 库 ==========
aux_source_directory(${PROJECT_SOURCE_DIR}/serviceModules/SignIn/src SignInSRC)
add_library(SignIn_static STATIC ${SignInSRC})
add_library(SignIn_shared SHARED ${SignInSRC})

# =========== 输出路径 ===========
set(LIBRARY_OUTPUT_PATH ../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

# ========== 主项目可执行程序 ==========
add_executable(${PROJECT_NAME} ./src/main.cpp)
target_link_libraries(${PROJECT_NAME}
    ${PROJECT_NAME}_static 
    workflow wfrest ssl crypto jwt  curl pthread SimpleAmqpClient srpc protobuf dl z lz4 snappy
)

# ======= 注册模块服务可执行程序 ========
if(EXISTS ${PROJECT_SOURCE_DIR}/serviceModules/SignUp/src/main.cpp)
    add_executable(SignUpServer ${PROJECT_SOURCE_DIR}/serviceModules/SignUp/src/main.cpp)
    target_link_libraries(SignUpServer
        SignUp_static
        # ${PROJECT_NAME}_static  
        srpc workflow protobuf ssl crypto pthread dl z lz4 snappy jwt
    )
endif()

# ======= 登录模块服务可执行程序 ========
if(EXISTS ${PROJECT_SOURCE_DIR}/serviceModules/SignIn/src/main.cpp)
    add_executable(SignInServer ${PROJECT_SOURCE_DIR}/serviceModules/SignIn/src/main.cpp)
    target_link_libraries(SignInServer
        SignIn_static
        # ${PROJECT_NAME}_static  
        srpc workflow protobuf ssl crypto pthread dl z lz4 snappy jwt
    )
endif()

# ========== Rabbit 可执行程序 =========
# 如果 Rabbit 有自己的 main.cpp
if(EXISTS ${PROJECT_SOURCE_DIR}/serviceModules/Rabbit/src/main.cpp)
    add_executable(Rabbit ${PROJECT_SOURCE_DIR}/serviceModules/Rabbit/src/main.cpp)
    target_link_libraries(Rabbit
        Rabbit_static
        # ${PROJECT_NAME}_static  
        workflow wfrest alibabacloud-oss-cpp-sdk ssl crypto jwt  curl pthread SimpleAmqpClient
    )
# else()
#     # 如果有单独的 Rabbit 程序源文件
#     # 创建一个专门用于 Rabbit 可执行程序的源文件列表
#     set(Rabbit_EXE_SRC ${RabbitSRC})
#     # 如果有专门的 main 文件，添加到列表
#     if(EXISTS ${PROJECT_SOURCE_DIR}/Rabbit/src/main_rabbit.cpp)
#         list(APPEND Rabbit_EXE_SRC ${PROJECT_SOURCE_DIR}/Rabbit/src/main_rabbit.cpp)
#     endif()
    
#     if(Rabbit_EXE_SRC)
#         add_executable(Rabbit ${Rabbit_EXE_SRC})
#         target_link_libraries(Rabbit
#             Rabbit_static
#             # ${PROJECT_NAME}_static
#             workflow wfrest alibabacloud-oss-cpp-sdk ssl crypto jwt  curl pthread SimpleAmqpClient
#         )
#     endif()
endif()

# # ========== 测试可执行程序 ==========
# # 如果有 Rabbit 的测试程序
# if(EXISTS ${PROJECT_SOURCE_DIR}/Rabbit/test/test_rabbit.cpp)
#     add_executable(test_rabbit ${PROJECT_SOURCE_DIR}/Rabbit/test/test_rabbit.cpp)
#     target_link_libraries(test_rabbit
#         Rabbit_static
#         ${PROJECT_NAME}_static
#         workflow wfrest alibabacloud-oss-cpp-sdk ssl crypto jwt  curl pthread gtest gtest_main SimpleAmqpClient
#     )
# endif()

# ========== 设置编译特性 ==========
target_compile_features(${PROJECT_NAME}_static PRIVATE cxx_std_17)
target_compile_features(${PROJECT_NAME}_shared PRIVATE cxx_std_17)
target_compile_features(Rabbit_static PRIVATE cxx_std_17)
target_compile_features(Rabbit_shared PRIVATE cxx_std_17)
target_compile_features(SignUp_static PRIVATE cxx_std_17)
target_compile_features(SignUp_shared PRIVATE cxx_std_17)
target_compile_features(SignIn_static PRIVATE cxx_std_17)
target_compile_features(SignIn_shared PRIVATE cxx_std_17)