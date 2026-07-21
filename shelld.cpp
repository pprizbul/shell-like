#include <array>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <asio.hpp>

class connection : public std::enable_shared_from_this<connection>{
public:
    typedef std::shared_ptr<connection> conn_ptr;

    static conn_ptr create(asio::io_context& io_context){
        return conn_ptr(new connection(io_context));
    }

    asio::ip::tcp::socket& socket(){
        return socket_;
    }

    void start(){
        socket_.async_read_some(asio::buffer(buffer_),
            std::bind(&connection::handle_read, shared_from_this(),
            asio::placeholders::error, asio::placeholders::bytes_transferred));
    } 

private:
    connection(asio::io_context& io_context)
    :  socket_(io_context){}

    void handle_read(const std::error_code& e, size_t bytes_transfered){
        if(e)
            std::cout << "Read error: " << e.message() << std::endl;

        recieved_message_.assign(buffer_.data(), bytes_transfered);
        std::cout << "Recieved message: " << recieved_message_ << std::endl;
        
        asio::async_write(socket_, asio::buffer(recieved_message_),
            std::bind(&connection::handle_write, shared_from_this()));
    }
            
    void handle_write(){
        std::cout << "Writing message ..." << std::endl;
    }
    
    asio::ip::tcp::socket socket_;
    std::array<char, 128> buffer_;
    std::string message_;
    std::string recieved_message_;
};

class server{
public:
    server(asio::io_context& io_context)
    :  io_context_(io_context),
       acceptor_(io_context, asio::ip::tcp::endpoint(
        asio::ip::tcp::v4(), 1301))
    {
        start_accept();
    }  
private:
    void start_accept(){
        connection::conn_ptr new_connection = 
            connection::create(io_context_);
        
        acceptor_.async_accept(new_connection->socket(),
            std::bind(&server::handle_accept, this, new_connection,
                asio::placeholders::error));
    }
    
    void handle_accept(connection::conn_ptr new_connection, 
        const std::error_code& error){
        if(!error)
            new_connection->start();
        start_accept();
    }
        
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
};

int main(){
    try{
        asio::io_context io_context;
        server server(io_context);
        io_context.run();
    }
    catch(std::exception &e){
        std::cerr << e.what() << std::endl;
    }
}