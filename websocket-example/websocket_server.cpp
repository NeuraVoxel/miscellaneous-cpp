//
// WebSocket服务器示例
// 使用Boost.Beast实现的WebSocket服务器，支持断点调试
//

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// 处理单个WebSocket连接的会话
class session : public std::enable_shared_from_this<session>
{
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;

public:
    // 接受TCP套接字的构造函数
    explicit session(tcp::socket socket)
        : ws_(std::move(socket))
    {
        std::cout << "创建新会话" << std::endl;
    }

    // 开始会话
    void run()
    {
        // 设置一个断点在这里可以查看新连接
        std::cout << "会话开始运行" << std::endl;
        
        // 设置WebSocket选项
        ws_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));

        // 设置最大消息大小限制
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-example");
            }));

        // 接受WebSocket握手
        ws_.async_accept(
            beast::bind_front_handler(
                &session::on_accept,
                shared_from_this()));
    }

private:
    void on_accept(beast::error_code ec)
    {
        if(ec)
        {
            std::cerr << "接受失败: " << ec.message() << std::endl;
            return;
        }

        // 设置一个断点在这里可以查看握手完成
        std::cout << "WebSocket握手成功" << std::endl;

        // 读取消息
        do_read();
    }

    void do_read()
    {
        // 读取消息
        ws_.async_read(
            buffer_,
            beast::bind_front_handler(
                &session::on_read,
                shared_from_this()));
    }

    void on_read(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // 处理可能的错误
        if(ec == websocket::error::closed)
        {
            std::cout << "连接已关闭" << std::endl;
            return;
        }

        if(ec)
        {
            std::cerr << "读取失败: " << ec.message() << std::endl;
            return;
        }

        // 获取接收到的消息
        std::string message = beast::buffers_to_string(buffer_.data());
        
        // 设置一个断点在这里可以查看接收到的消息
        std::cout << "收到消息: " << message << std::endl;
        
        // 清除缓冲区
        buffer_.consume(buffer_.size());

        // 回显消息
        ws_.async_write(
            net::buffer("服务器回显: " + message),
            beast::bind_front_handler(
                &session::on_write,
                shared_from_this()));
    }

    void on_write(
        beast::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
        {
            std::cerr << "写入失败: " << ec.message() << std::endl;
            return;
        }

        // 设置一个断点在这里可以查看消息发送完成
        std::cout << "消息已发送" << std::endl;

        // 继续读取下一条消息
        do_read();
    }
};

// 接受传入连接并启动会话的监听器
class listener : public std::enable_shared_from_this<listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    listener(
        net::io_context& ioc,
        tcp::endpoint endpoint)
        : ioc_(ioc)
        , acceptor_(ioc)
    {
        beast::error_code ec;

        // 打开acceptor
        acceptor_.open(endpoint.protocol(), ec);
        if(ec)
        {
            std::cerr << "打开失败: " << ec.message() << std::endl;
            return;
        }

        // 允许地址重用
        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if(ec)
        {
            std::cerr << "设置选项失败: " << ec.message() << std::endl;
            return;
        }

        // 绑定到端点
        acceptor_.bind(endpoint, ec);
        if(ec)
        {
            std::cerr << "绑定失败: " << ec.message() << std::endl;
            return;
        }

        // 开始监听连接
        acceptor_.listen(
            net::socket_base::max_listen_connections, ec);
        if(ec)
        {
            std::cerr << "监听失败: " << ec.message() << std::endl;
            return;
        }
    }

    // 开始接受传入连接
    void run()
    {
        // 设置一个断点在这里可以查看服务器启动
        std::cout << "服务器开始监听连接" << std::endl;
        
        do_accept();
    }

private:
    void do_accept()
    {
        // 异步接受连接
        acceptor_.async_accept(
            net::make_strand(ioc_),
            beast::bind_front_handler(
                &listener::on_accept,
                shared_from_this()));
    }

    void on_accept(beast::error_code ec, tcp::socket socket)
    {
        if(ec)
        {
            std::cerr << "接受失败: " << ec.message() << std::endl;
        }
        else
        {
            // 设置一个断点在这里可以查看新连接
            std::cout << "接受新连接" << std::endl;
            
            // 创建会话并运行它
            std::make_shared<session>(std::move(socket))->run();
        }

        // 接受下一个连接
        do_accept();
    }
};

int main(int argc, char* argv[])
{
    // 检查命令行参数
    if (argc != 3)
    {
        std::cerr << "用法: websocket_server <地址> <端口>\n"
                  << "示例:\n"
                  << "    websocket_server 0.0.0.0 8080\n";
        return EXIT_FAILURE;
    }
    
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));

    // 设置一个断点在这里可以查看服务器配置
    std::cout << "服务器配置: " << address << ":" << port << std::endl;

    // IO上下文
    net::io_context ioc{1};

    // 创建并运行监听器
    std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();

    // 捕获SIGINT信号
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&](beast::error_code const&, int)
        {
            // 停止io_context
            ioc.stop();
        });

    // 运行IO服务
    std::cout << "服务器启动，按Ctrl+C退出" << std::endl;
    ioc.run();

    // 设置一个断点在这里可以查看服务器关闭
    std::cout << "服务器已关闭" << std::endl;

    return EXIT_SUCCESS;
}