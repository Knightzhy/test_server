#include <iostream>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

void print(mongocxx::collection &col)
{
    bsoncxx::stdx::optional<bsoncxx::document::value> res = col.find_one({});
    if (res)
        std::cout << bsoncxx::to_json(*res) << std::endl;
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
    mongocxx::uri yace_uri{"mongodb://root:123456@172.16.16.6:27017"};

    mongocxx::options::client client_options;
    if (yace_uri.ssl()) {
        std::cout << "ssl-----------\n";
    }
    else {
        std::cout << "no-ssl\n";
    }

    mongocxx::client conn{yace_uri};

    mongocxx::database db = conn["runoob"];
    mongocxx::collection col = db["runoob"];
 //   insert(col);

    print(col);

    return 0;
}

/*
 *
  g++ -o a test.cpp -I /home/knightwu/mongocxx/include/bsoncxx/v_noabi -I /home/knightwu/mongocxx/include/mongocxx/v_noabi -std=c++11 -L/home/knightwu/mongocxx/lib64 -lbsoncxx -lmongocxx
 * */
