#!/bin/bash

LOG_DIR="./log"
BIN_DIR="./bin"

stop_services() {
    echo "All services are being stopped...."
    
    # 停止主服务器
    if pkill -f "CouldServer"; then
        echo "The main server has stopped."
    else
        echo "The main server is not running."
    fi
    
    # 停止注册服务器
    if pkill -f "SignUpServer"; then
        echo "The SignUpServer has stopped."
    else
        echo "The SignUpServer is not running."
    fi

    # 停止登录服务器
    if pkill -f "SignInServer"; then
        echo "The SignInServer has stopped."
    else
        echo "The SignInServer is not running."
    fi
    
    # 停止Rabbit程序
    if pkill -f "Rabbit"; then
        echo "The Rabbit program has stopped."
    else
        echo "The Rabbit program is not running."
    fi
    
    echo "All services have been stopped."
}

status_services() {
    echo "Service status check:"
    echo "================"
    
    local could_pid=$(pgrep -f "CouldServer")
    local signup_pid=$(pgrep -f "SignUpServer")
    local signin_pid=$(pgrep -f "SignInServer")
    local rabbit_pid=$(pgrep -f "Rabbit")
    
    if [ -n "$could_pid" ]; then
        echo "The main server is currently running. (PID: $could_pid)"
        echo "log: $LOG_DIR/couldserver.log"
    else
        echo "The main server is not running."
    fi
    
    if [ -n "$signup_pid" ]; then
        echo "The SignUpServer is currently running. (PID: $signup_pid)"
        echo "log: $LOG_DIR/signup.log"
    else
        echo "The SignUpServer is not running."
    fi

    if [ -n "$signup_pid" ]; then
        echo "The SignInServer is currently running. (PID: $signin_pid)"
        echo "log: $LOG_DIR/signin.log"
    else
        echo "The SignInServer is not running"
    fi
    
    if [ -n "$rabbit_pid" ]; then
        echo "The Rabbit program is running. (PID: $rabbit_pid)"
        echo "log: $LOG_DIR/rabbit.log"
    else
        echo "The Rabbit program is not running."
    fi
}

start_services() {
    echo "All services are being started...."
    
    # 检查日志目录
    if [ ! -d "$LOG_DIR" ]; then
        echo "Create log directory: $LOG_DIR"
        mkdir -p "$LOG_DIR"
    fi
    
    # 检查可执行文件
    if [ ! -f "$BIN_DIR/CouldServer" ]; then
        echo "error: $BIN_DIR/CouldServer does not exist"
        exit 1
    fi
    if [ ! -f "$BIN_DIR/SignUpServer" ]; then
        echo "error: $BIN_DIR/SignUpServer does not exist"
        exit 1
    fi
    if [ ! -f "$BIN_DIR/SignInServer" ]; then
        echo "error: $BIN_DIR/SignInServer does not exist"
        exit 1
    fi
    if [ ! -f "$BIN_DIR/Rabbit" ]; then
        echo "error: $BIN_DIR/Rabbit does not exist"
        exit 1
    fi
    
    # 运行主服务器
    nohup $BIN_DIR/CouldServer > $LOG_DIR/couldserver.log 2>&1 &
    local could_pid=$!
    echo "The main server has been started. (PID: $could_pid)"
    
    # 运行注册服务器
    nohup $BIN_DIR/SignUpServer > $LOG_DIR/signup.log 2>&1 &
    local signup_pid=$!
    echo "The SignUpServer has been started. (PID: $signup_pid)"
    
    # 运行Rabbit程序
    nohup $BIN_DIR/Rabbit > $LOG_DIR/rabbit.log 2>&1 &
    local rabbit_pid=$!
    echo "The Rabbit program has been launched. (PID: $rabbit_pid)"

    # 运行SignInServer程序
    nohup $BIN_DIR/SignInServer > $LOG_DIR/signin.log 2>&1 &
    local signin_pid=$!
    echo "The SignInServer program has been started. (PID: $signin_pid)"
    
    echo "All services have been successfully started."
    echo "================"
    echo "Log Directory: $LOG_DIR"
    echo "Couldserver log: $LOG_DIR/couldserver.log"
    echo "SignupServer log: $LOG_DIR/signup.log"
    echo "SigninServer log: $LOG_DIR/signin.log"
    echo "Rabbit Program log: $LOG_DIR/rabbit.log"
}

restart_services() {
    stop_services
    echo ""
    echo "wait 2s..."
    sleep 2
    echo ""
    start_services
}

view_logs() {
    case "$2" in
        could|couldserver)
            echo "查看主服务器日志:"
            echo "================="
            tail -f $LOG_DIR/couldserver.log
            ;;
        signup)
            echo "查看注册服务器日志:"
            echo "=================="
            tail -f $LOG_DIR/signup.log
            ;;
        rabbit)
            echo "查看Rabbit程序日志:"
            echo "=================="
            tail -f $LOG_DIR/rabbit.log
            ;;
        signin)
            echo "查看signin程序日志:"
            echo "=================="
            tail -f $LOG_DIR/signin.log
            ;;
        *)
            echo "使用方法: $0 log {could|signup|rabbit}"
            echo "  可选的日志:"
            echo "    could    - 主服务器日志"
            echo "    signup   - 注册服务器日志"
            echo "    rabbit   - Rabbit程序日志"
            echo "    signin   - signin程序日志"
            exit 1
            ;;
    esac
}

# 根据参数执行不同操作
case "$1" in
    start)
        start_services
        ;;
    stop)
        stop_services
        ;;
    status)
        status_services
        ;;
    restart)
        restart_services
        ;;
    log)
        view_logs "$@"
        ;;
    *)
        echo ""
        echo "Usage: $0 {start|stop|status|restart|log}"
        echo "Generate output based on the options given:"
        echo "  start    Start all services"
        echo "  stop     Stop all services"
        echo "  status   Check service status"
        echo "  restart  Restart all services"
        echo "  log      View logs (requires a second parameter)"
        echo ""
        echo "Log viewing example:"
        echo "  $0 log could     Check the main server log"
        echo "  $0 log signup    Check the registration server log"
        echo "  $0 log rabbit    Check the log file of the Rabbit program"
        exit 1
        ;;
esac
