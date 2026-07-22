#include <array>
#include <iostream>
#include <memory>
#include <asio.hpp>

class client : public std::enable_shared_from_this<client>{
public:
    typedef std::shared_ptr<client> client_ptr;

    static client_ptr create(asio::io_context& io_context, 
        asio::ip::tcp::resolver::results_type& endpoints){
            return client_ptr(new client(io_context, endpoints));
        }

    void start(){
        do_connect();
    }
private:
    client(asio::io_context& io_context, 
        asio::ip::tcp::resolver::results_type& endpoints)
    :  io_context_(io_context),
        endpoints_(endpoints),
        socket_(io_context)
        {
        }

    void do_connect(){
        asio::connect(socket_, endpoints_);
        do_read();
    }

    /* void do_write(){
        auto self(shared_from_this());
        std::string imessage{"test_message"};
        std::cout << "Writing message " << imessage << std::endl;
       
        socket_.async_write_some(asio::buffer(imessage),
        [this, self](const asio::error_code, std::size_t){
            std::cout << "Message sent" << std::endl;
            do_read();
        });
    } */

    void do_read(){
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(buffer_),
        [this, self](asio::error_code, std::size_t bytes_transfered){
            std::cout.write(buffer_.data(), bytes_transfered) << std::flush;
            do_read();
        });
    }

    asio::io_context& io_context_;
    asio::ip::tcp::resolver::results_type& endpoints_;
    asio::ip::tcp::socket socket_;
    std::array<char, 128> buffer_;
    std::string message_;
};

int main(int argc, char *argv[]) {
    try{
        if(argc != 2){
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }

        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        asio::ip::tcp::resolver::results_type endpoints =
            resolver.resolve(argv[1], "1301");
        
        client::client_ptr new_client = client::create(io_context, endpoints);
        new_client->start();
        io_context.run();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}