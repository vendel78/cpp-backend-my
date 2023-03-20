#include <fstream>
#include "json_loader.h"

namespace json_loader {

model::Game LoadGame(const std::filesystem::path& json_path) {
    // Загрузить содержимое файла json_path, например, в виде строки
    // Распарсить строку как JSON, используя boost::json::parse
    // Загрузить модель игры из файла
    model::Game game;
    std::ifstream fs(json_path.native());
    std::ostringstream sstr;
    sstr << fs.rdbuf();
    boost::json::error_code ec;
    auto doc = boost::json::parse(sstr.str(), ec);

    for (auto& map : doc.at("maps").as_array()) {
        std::cout << " --- " << map << "\n";
        std::string _id, _name;
        _id = map.as_object().at("id").as_string();
        _name = map.as_object().at("name").as_string();
        model::Map::Id id=model::Map::Id(_id);
        model::Map c_map=model::Map{id,_name};
        for (auto& key : map.as_object()) {
            std::cout << key.key() << ": " << key.value() << "\n";
            if (key.key().compare("id")==0) {
            } else if (key.key().compare("name")==0) {
            } else if (key.key().compare("roads")==0) {
                for (auto& road : key.value().as_array()) {
                    std::cout << " --- " << road << "\n";
                    int x0,y0;
                    for (auto& key : road.as_object()) {
                        std::cout << key.key() << ": " << key.value() << "\n";
                        if (key.key().compare("x0") == 0) {
                            x0 = key.value().as_int64();
                        } else if (key.key().compare("y0") == 0) {
                            y0 = key.value().as_int64();
                        } else if (key.key().compare("x1") == 0) {
                            int x1 = key.value().as_int64();
                            c_map.AddRoad(model::Road{model::Road::HORIZONTAL,
                                                      model::Point{x0,y0},x1});
                        } else if (key.key().compare("y1") == 0) {
                            int y1 = key.value().as_int64();
                            c_map.AddRoad(model::Road{model::Road::VERTICAL,
                                                      model::Point{x0,y0},y1});
                        }
                    }
                }
            } else if (key.key().compare("buildings")==0) {
                for (auto& building : key.value().as_array()) {
                    std::cout << " --- " << building << "\n";
                    int x,y,w,h;
                    for (auto& key : building.as_object()) {
                        std::cout << key.key() << ": " << key.value() << "\n";
                        if (key.key().compare("x") == 0) {
                            x = key.value().as_int64();
                        } else if (key.key().compare("y") == 0) {
                            y = key.value().as_int64();
                        } else if (key.key().compare("w") == 0) {
                            w = key.value().as_int64();
                        } else if (key.key().compare("h") == 0) {
                            h = key.value().as_int64();
                        }
                    }
                    c_map.AddBuilding(model::Building{
                            {{x,y},{w,h}}});
                }
            } else if (key.key().compare("offices")==0) {
                for (auto& office : key.value().as_array()) {
                    std::cout << " --- " << office << "\n";
                    std::string id;
                    int x,y,offsetX,offsetY;
                    for (auto& key : office.as_object()) {
                        std::cout << key.key() << ": " << key.value() << "\n";
                        if (key.key().compare("id") == 0) {
                            id = key.value().as_string();
                        } else if (key.key().compare("x") == 0) {
                            x = key.value().as_int64();
                        } else if (key.key().compare("y") == 0) {
                            y = key.value().as_int64();
                        } else if (key.key().compare("offsetX") == 0) {
                            offsetX = key.value().as_int64();
                        } else if (key.key().compare("offsetY") == 0) {
                            offsetY = key.value().as_int64();
                        }
                    }
                    c_map.AddOffice(model::Office{model::Office::Id(id),
                                                  {x,y},{offsetX,offsetY}});
                }
            } else {

            }
        }
        game.AddMap(c_map);
    }

    return game;
}

}  // namespace json_loader
