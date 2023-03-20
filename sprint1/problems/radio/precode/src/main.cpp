#include "audio.h"
#include <iostream>
#include <boost/asio.hpp>
#include <array>
#include <iostream>
#include <string>
#include <string_view>

namespace net = boost::asio;
using net::ip::udp;
using namespace std::literals;

void StartServer(uint16_t port)
{
    static const size_t max_buffer_size = 65000;
    Player player(ma_format_u8, 1);

    try {
        boost::asio::io_context io_context;
        udp::socket socket(io_context, udp::endpoint(udp::v4(), port));
        // Запускаем сервер в цикле, чтобы можно было работать со многими клиентами
        for (;;) {
            // Создаём буфер достаточного размера, чтобы вместить датаграмму.
            std::array<char, max_buffer_size> recv_buf;
            udp::endpoint remote_endpoint;
            // Получаем не только данные, но и endpoint клиента
            auto size = socket.receive_from(boost::asio::buffer(recv_buf), remote_endpoint);
            std::cout << "Playing start" << std::endl;
            player.PlayBuffer(recv_buf.data(), size / player.GetFrameSize(), 1.5s);
            std::cout << "Playing done" << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

void StartClient(char* servAddr, uint16_t port) 
{
    static const size_t max_buffer_size = 65000;
    try {
        Recorder recorder(ma_format_u8, 1);

        while (true) {
            std::string str;

            std::cout << "Press Enter to record message..." << std::endl;
            std::getline(std::cin, str);

            auto rec_result = recorder.Record(65000, 1.5s);
            std::cout << "Recording done" << std::endl;

            net::io_context io_context;
            // Перед отправкой данных нужно открыть сокет. 
            // При открытии указываем протокол (IPv4 или IPv6) вместо endpoint.
            udp::socket socket(io_context, udp::v4());
            boost::system::error_code ec;
            auto endpoint = udp::endpoint(net::ip::make_address(servAddr, ec), port);

            socket.send_to(net::buffer(rec_result.data.data(), rec_result.frames * recorder.GetFrameSize()), endpoint);
            std::cout << "Data sended" << std::endl;
        }
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) return -1;
    std::string arg1 = argv[1];
    if (arg1.compare("server") == 0) {
        StartServer(3333);
    }
    else if (arg1.compare("client") == 0 && argc == 3) {
        StartClient(argv[2], 3333);
    }
    return 0;
}

