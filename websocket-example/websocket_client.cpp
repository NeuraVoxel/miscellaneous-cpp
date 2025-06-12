//
// WebSocket客户端示例
// 使用Boost.Beast实现的WebSocket客户端，支持断点调试
//

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// WebSocket客户端会话
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    std::string host_;
    std::string message_;

public:
    // 构造函数
    explicit session(net::io_context& ioc)
        : resolver_(net::make_strand(ioc))
        , ws_(net::make_strand(ioc))
    {
        std::cout << "创建客户端会话" << std::endl;
    }

    // 启动WebSocket会话
    void run(
        char const* host,
        char const* port,
        char const* text)
    {
        // 保存这些以供后面使用
        host_ = host;
        message_ = text;

        // 设置一个断点在这里可以查看客户端配置
        std::cout << "客户端配置: " << host << ":" << port << std::endl;
        std::cout << "准备发送消息: " << message_ << std::endl;

        // 查找域名
        resolver_.async_resolve(
            host,
            port,
            beast::bind_front_handler(
                &session::on_resolve,
                shared_from_this()));
    }

private:
    void on_resolve(
        beast::error_code ec,
        tcp::resolver::results_type results)
    {
        if(ec)
        {
            std::cerr << "解析失败: " << ec.message() << std::endl;
            return;
        }

        // 设置一个断点在这里可以查看解析结果
        std::cout << "域名解析成功" << std::endl;

        // 连接到服务器
        beast::get_lowest_layer(ws_).async_connect(
            results,
            beast::bind_front_handler(
                &session::on_connect,
                shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
    {
        if(ec)
        {
            std::cerr << "连接失败: " << ec.message() << std::endl;
            return;
        }

        // 设置一个断点在这里可以查看连接成功
        std::cout << "连接成功: " << ep.address().to_string() << ":" << ep.port() << std::endl;

        // 更新主机名（用于HTTP握手）
        host_ += ':' + std::to_string(ep.port());

        // 执行WebSocket握手
        ws_.async_handshake(host_, "/",
            beast::bind_front_handler(
                &session::on_handshake,
                shared_from_this()));
    }

    void on_handshake(beast::error_code ec)
    {
        if(ec)
        {
            std::cerr << "握手失败: " << ec.message() << std::endl;
            return;
        }

        // 设置一个断点在这里可以查看握手成功
        std::cout << "WebSocket握手成功" << std::endl;

        // 发送消息
        ws_.async_write(
            net::buffer(message_),
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

        // 设置一个断点在这里可以查看消息发送成功
        std::cout << "消息已发送: " << message_ << std::endl;

        // 读取服务器响应
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

        if(ec)
        {
            std::cerr << "读取失败: " << ec.message() << std::endl;
            return;
        }

        // 获取接收到的消息
        std::string response = beast::buffers_to_string(buffer_.data());
        
        // 设置一个断点在这里可以查看接收到的响应
        std::cout << "收到响应: " << response << std::endl;

        // 关闭WebSocket连接
        ws_.async_close(websocket::close_code::normal,
            beast::bind_front_handler(
                &session::on_close,
                shared_from_this()));
    }

    void on_close(beast::error_code ec)
    {
        if(ec)
        {
            std::cerr << "关闭失败: " << ec.message() << std::endl;
            return;
        }

        // 设置一个断点在这里可以查看连接关闭
        std::cout << "连接已关闭" << std::endl;
    }
};

int main(int argc, char** argv)
{
    // 检查命令行参数
    if(argc != 4)
    {
        std::cerr << "用法: websocket_client <主机> <端口> <消息>\n"
                  << "示例:\n"
                  << "    websocket_client localhost 8080 \"Hello, WebSocket!\"\n";
        return EXIT_FAILURE;
    }
    
    auto const host = argv[1];
    auto const port = argv[2];
    auto const message = argv[3];

    // IO上下文
    net::io_context ioc;

    // 启动WebSocket会话
    std::make_shared<session>(ioc)->run(host, port, message);

    // 运行IO服务
    std::cout << "客户端启动" << std::endl;
    ioc.run();

    // 设置一个断点在这里可以查看客户端退出
    std::cout << "客户端已退出" << std::endl;

    return EXIT_SUCCESS;
}