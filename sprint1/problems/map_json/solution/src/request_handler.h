#pragma once
#include "http_server.h"
#include "model.h"

namespace http_handler {
    namespace beast = boost::beast;
    namespace http = beast::http;

    // Запрос, тело которого представлено в виде строки
    using StringRequest = http::request<http::string_body>;
// Ответ, тело которого представлено в виде строки
    using StringResponse = http::response<http::string_body>;

    struct ContentType {
        ContentType() = delete;

        constexpr static std::string_view TEXT_HTML = "text/html"sv;
        constexpr static std::string_view APP_JSON = "application/json"sv;
        // При необходимости внутрь ContentType можно добавить и другие типы контента
    };

    class RequestHandler {
    public:
        explicit RequestHandler(model::Game &game)
                : game_{game} {
        }
        RequestHandler(const RequestHandler &) = delete;
        RequestHandler &operator=(const RequestHandler &) = delete;

        template<typename Body, typename Allocator, typename Send>
        void operator()(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send) {
            // Обработать запрос request и отправить ответ, используя send
            std::string res;
            if (req.method() == http::verb::get || req.method() == http::verb::head) {
                std::string str=std::string(req.target());
                http::status status=http::status::ok;
                if (str.compare(0,13,"/api/v1/maps/")==0) {
                    std::string _id=str;
                    _id.erase(0, 13);
                    model::Map::Id id=model::Map::Id(_id);
                    auto map = game_.FindMap(id);
                    if (map) {
                        res = "{\n";
                        res += "  \"id\": \""+*map->GetId()+"\",\n";
                        res += "  \"name\": \""+map->GetName()+"\",\n";
                        res += "  \"roads\": [\n";
                        bool first=true;
                        for (auto& road : map->GetRoads()) {
                            if (!first) res+=",\n";
                            first= false;
                            if (road.IsHorizontal()) {
                                res += "    { \"x0\": " + std::to_string(road.GetStart().x);
                                res += ", \"y0\": " + std::to_string(road.GetStart().y);
                                res += ", \"x1\": " + std::to_string(road.GetEnd().x);
                                res += " }";
                            } else {
                                res += "    { \"x0\": " + std::to_string(road.GetStart().x);
                                res += ", \"y0\": " + std::to_string(road.GetStart().y);
                                res += ", \"y1\": " + std::to_string(road.GetEnd().y);
                                res += " }";
                            }
                        }
                        res += "\n  ],\n  \"buildings\": [\n";
                        first=true;
                        for (auto& building : map->GetBuildings()) {
                            if (!first) res+=",\n";
                            first= false;
                            res += "    { \"x\": " + std::to_string(building.GetBounds().position.x);
                            res += ", \"y\": " + std::to_string(building.GetBounds().position.y);
                            res += ", \"w\": " + std::to_string(building.GetBounds().size.width);
                            res += ", \"h\": " + std::to_string(building.GetBounds().size.height);
                            res += " }";
                        }
                        res += "\n  ],\n  \"offices\": [\n";
                        first=true;
                        for (auto& office : map->GetOffices()) {
                            if (!first) res+=",\n";
                            first= false;
                            res += "    { \"id\": \"" + *office.GetId()+"\"";
                            res += ", \"x\": " + std::to_string(office.GetPosition().x);
                            res += ", \"y\": " + std::to_string(office.GetPosition().y);
                            res += ", \"offsetX\": " + std::to_string(office.GetOffset().dx);
                            res += ", \"offsetY\": " + std::to_string(office.GetOffset().dy);
                            res += " }";
                        }
                        res += "\n  ]\n}\n";
                    } else {
                        status=http::status::not_found;
                        res="{\n"
                            "  \"code\": \"mapNotFound\",\n"
                            "  \"message\": \"Map not found\"\n"
                            "}";
                    }
                }
                else if (str=="/api/v1/maps") {
                    res = "[";
                    for (auto& map : game_.GetMaps()) {
                        res+="{\"id\": \""+*map.GetId()+"\", \"name\": \""+
                                map.GetName()+"\"}";
                    }
                    res += "]";
                }
                else if (str.compare(0,5,"/api/")==0) {
                    status=http::status::bad_request;
                    res="{\n"
                        "  \"code\": \"badRequest\",\n"
                        "  \"message\": \"Bad request\"\n"
                        "} ";
                }
                const auto text_response = [&req](http::status status, std::string_view text) {
                    return MakeStringResponse(status, text, req.version(), req.keep_alive());
                };
                send(text_response(status, res));
            } else {
                const auto text_response = [&req](http::status status, std::string_view text) {
                    return MakeStringResponse(status, text, req.version(), req.keep_alive());
                };
                res = "Invalid method";
                send(text_response(http::status::method_not_allowed, res));
            }
        }

    private:
// Создаёт StringResponse с заданными параметрами
        static StringResponse MakeStringResponse(http::status status, std::string_view body, unsigned http_version,
                                          bool keep_alive,
                                          std::string_view content_type = ContentType::APP_JSON) {
            StringResponse response(status, http_version);
            response.set(http::field::content_type, content_type);
            if (status == http::status::method_not_allowed) {
                response.set(http::field::allow, "GET, HEAD"sv);
            }
            response.body() = body;
            response.content_length(body.size());
            response.keep_alive(keep_alive);
            return response;
        }

        model::Game &game_;
    };

}  // namespace http_handler
