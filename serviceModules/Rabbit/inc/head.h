#ifndef HEAD_H
#define HEAD_H

// 输入输出流
#include <iostream>    // cin, cout, cerr, clog
#include <fstream>     // 文件流
#include <sstream>     // 字符串流
#include <iomanip>     // 输入输出格式化

// 容器类
#include <vector>      // 动态数组
#include <list>        // 双向链表
#include <deque>       // 双端队列
#include <array>       // 固定大小数组
#include <forward_list>// 单向链表

// 关联容器
#include <set>         // 集合
#include <map>         // 映射
#include <unordered_set> // 无序集合
#include <unordered_map> // 无序映射

// 字符串处理
#include <string>      // 字符串类
#include <cstring>     // C风格字符串函数
#include <cctype>      // 字符处理函数

// 算法和数值
#include <algorithm>   // 常用算法
#include <numeric>     // 数值算法
#include <functional>  // 函数对象
#include <cmath>       // 数学函数
#include <complex>     // 复数
#include <random>      // 随机数

// 内存管理
#include <memory>      // 智能指针
#include <new>         // 动态内存管理

// 实用工具
#include <utility>     // pair, move, forward
#include <tuple>       // 元组
#include <optional>    // 可选值 (C++17)
#include <variant>     // 变体 (C++17)
#include <any>         // 任意类型 (C++17)

// 时间和日期
#include <chrono>      // 时间库
#include <ctime>       // C风格时间

// 异常处理
#include <exception>   // 异常基类
#include <stdexcept>   // 标准异常

// 类型支持
#include <type_traits> // 类型特征
#include <typeinfo>    // 类型信息
#include <typeindex>   // 类型索引

// 多线程
#include <thread>      // 线程
#include <mutex>       // 互斥锁
#include <atomic>      // 原子操作
#include <future>      // 异步操作
#include <condition_variable> // 条件变量

// 文件系统 (C++17)
#include <filesystem>

// 其他
#include <limits>      // 数值极限
#include <cassert>     // 断言
#include <cstdlib>     // C标准库
#include <cstddef>     // 标准定义
#include <cstdint>     // 固定宽度整数类型
#endif
