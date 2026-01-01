# DataStorage
It's just some of my wild ideas. I don't know what use they will be,now.

It still has many ideas that have not been realized. As my abilities improve, I gradually bring them to fruition. It is merely used to record my growth.

## Preparation

1. **Design of database tables（MySQL）**

   1. user table

      ```sql
      CREATE TABLE `tbl_user` (
        `id` int NOT NULL AUTO_INCREMENT,
        `username` varchar(255) NOT NULL,
        `password` varchar(255) NOT NULL,
        `salt` varchar(64) NOT NULL,
        `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
        `tomb` int DEFAULT '0',
        PRIMARY KEY (`id`),
        UNIQUE KEY `username` (`username`)
      ) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci
      ```

   2. file table

      ```sql
      CREATE TABLE `tbl_file` (
        `id` int NOT NULL AUTO_INCREMENT,
        `uid` int NOT NULL,
        `filename` varchar(255) NOT NULL,
        `hashcode` varchar(255) NOT NULL,
        `size` bigint DEFAULT '0',
        `created_at` datetime DEFAULT CURRENT_TIMESTAMP,
        `last_update` datetime DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
        `status` int DEFAULT '0' COMMENT 'Status (available/disabled/deleted, etc.)',
        PRIMARY KEY (`id`)
      ) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci
      ```

2. Alibaba Cloud OSS Storage

   1. download and install [OSS C++ SDK](https://github.com/aliyun/aliyun-oss-cpp-sdk)

      ```shell
      # Install dependent libraries
      $ sudo apt install libssl-dev
      $ sudo apt install libcurl4-openssl-dev
      
      # Decompress the installation package of the C++ SDK
      $ tar xzvf aliyun-oss-cpp-sdk-1.10.0.tar.gz
      $ cd aliyun-oss-cpp-sdk-1.10.0
      
      # Install C++ SDK
      $ mkdir build
      $ cd build
      $ cmake ..
      $ make 
      $ sudo make install 
      $ sudo ldconfig
      ```

   2. The default method of using hard links **(which is highly discouraged by the official)** involves modifying all the AK parameters in **Config.cpp**. Of course, I have provided a method for using custom credentials. If you need to log in using a Token **(which is also the official recommended method)**, please modify the specific implementation of **CustomCredentialsProvider::getCredentials()** in OssCustom.cpp

3. Install RabbitMQ using Docker

   ```shell
   $ sudo docker pull rabbitmq:management
   $ sudo docker run -d --hostname rabbitsrv --name rabbit -p 5672:5672 -p 15672:15672 -p 25672:25672 -v /data/rabbitmq:/var/lib/rabbitmq rabbitmq:management
   ```

   **Note**: Perhaps you don't need "sudo" privileges. Of course,Your mount path settings can be different from mine.

4. Install [SimpleAmqpClient](https://github.com/alanxz/SimpleAmqpClient)

   dependency(rely on):

   ```shell
   $ sudo apt install libboost-dev    		
   $ sudo apt install libboost-chrono-dev
   $ sudo apt install libboost-system-dev
   ```

5. Install [Protobuf](https://github.com/protocolbuffers/protobuf)

6. Install [Sogou C++ Workflow](https://github.com/sogou/workflow)

7. Install [wfrest](https://github.com/wfrest/wfrest)

8. Install [sRPC](https://github.com/sogou/srpc) 

   dependency(rely on):

   ```shell
   $ sudo apt install liblz4-dev
   $ sudo apt install libsnappy-dev
   ```

9. Install Consul using Docker

   ```shell
   $ sudo docker run --name consul1 -d -p 8500:8500 -p 8301:8301 -p 8302:8302 -p 8600:8600 \
   hashicorp/consul agent -server -bootstrap-expect 2 -ui -bind=0.0.0.0 -client=0.0.0.0
   
   # Used to view the IP address of node one.
   # Please remember it.
   # It will be used to replace the curly braces {consul1 IP}.
   $ sudo docker inspect consul1
   
   $ sudo docker run --name consul2 -d -p 8501:8500 hashicorp/consul agent -server -ui \
   -bind=0.0.0.0 -client=0.0.0.0 -join {consul1 IP}
   
   $ sudo docker run --name consul3 -d -p 8502:8500 hashicorp/consul agent -server -ui \
   -bind=0.0.0.0 -client=0.0.0.0 -join {consul1 IP}
   ```

10. Install [ppconsul](https://github.com/oliora/ppconsul)

Note:

```c
// Consul Homepage
http://<consul_host>:8500
// Instances used for checking the health of services
http://<consul_host>:8500/v1/health/service/<service_name>?passing=true
```

## Introduction

At present, a basic function of data storage has been implemented.

## How to use it？

1. Install

   ```shell
   $ ./cmake_make.sh
   ```

2. Usage

   ```shell
   $ ./service_manage.sh {start|stop|status|restart|log}
   
   Generate output based on the options given:
     start		Start all services
     stop      Stop all services
     status	Check service status
     restart	Restart all services
     log		View logs (requires a second parameter)
   ```
   

## **Future plan to implement functions**

**TODO**:

1. Log collection service.
2. Load balancing(Server Load Balancing or Dynamic Load Balancing,I have no idea which method to use. Try it and see how it goes :) ).
3. API Gateway Cluster? Perhaps it will come true.
4. Log analysis and warning notifications
5. Damn that web page! But I can do nothing about it. Let's talk about it later.


