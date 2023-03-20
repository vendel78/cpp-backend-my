#include "http_server.h"

#include <boost/asio/dispatch.hpp>
#include <iostream>

namespace http_server {}  // namespace http_server

void ReportError(sys::error_code ec, std::string_view what){
    std::cerr << what << ": "sv << ec.message() << std::endl;
}
