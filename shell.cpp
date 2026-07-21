#include <array>
#include <iostream>
#include <asio.hpp>

void handle_read(const std::error_code& e, 
    size_t bytes_transfered, std::string *recieved_message);
void handle_write(asio::ip::tcp::socket *socket);

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

        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::array<char, 128> buf;
        std::error_code error;

        socket.async_write_some(asio::buffer("test"), 
            std::bind(&handle_write, &socket));
        
        io_context.run();
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

void handle_read(const std::error_code& e, 
    size_t bytes_transfered, std::string *recieved_message){
    std::cout << "Recieved message" << std::endl;
}

void handle_write(asio::ip::tcp::socket *socket){
    std::cout << "Writing message " << "test" << std::endl;

    std::string recieved_message;
    socket->async_read_some(asio::buffer(recieved_message),
        std::bind(&handle_read, asio::placeholders::error, 
            asio::placeholders::bytes_transferred, &recieved_message));        
}