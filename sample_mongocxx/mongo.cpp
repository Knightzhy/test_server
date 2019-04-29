#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

void print(mongocxx::collection &col)
{
    mongocxx::cursor cursor = col.find({});
    for (auto doc :cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}

void insert(mongocxx::collection &col)
{
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
        << "name" << "BBBBB"
        << "type" << "database"
        << "count" << 1
        << "versions" << bsoncxx::builder::stream::open_array
        << "v3.2" << "v3.0" << "v2.6"
        << bsoncxx::builder::stream::close_array
        << "info" << bsoncxx::builder::stream::open_document
        << "x" << 203
        << "y" << 102
        << bsoncxx::builder::stream::close_document
        << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view_or_value d(doc_value);
    col.insert_one(d);
}

int main(int, char**)
{
    mongocxx::instance inst{};
    //mongocxx::uri yace_uri{"mongodb://test_username:test_pwd@test_ip:test_port/?authSource=test_db"};
    mongocxx::uri yace_uri{"mongodb://"};
    mongocxx::pool p(yace_uri);
    mongocxx::pool::entry e = p.acquire();
    mongocxx::client &c = *e;

    mongocxx::database db = c["runoob"];
    mongocxx::collection col = db["runoob"];
    print(col);
    e = 0;
    return 0;
    if (yace_uri.ssl()) {
        std::cout << "ssl-----------\n";
    }
    else {
        std::cout << "no-ssl\n";
    }

    mongocxx::client conn{yace_uri};

    //mongocxx::database db = conn["runoob"];
    //mongocxx::collection col = db["runoob"];
 //   insert(col);

    print(col);

    return 0;
}
