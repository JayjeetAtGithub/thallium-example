#include <iostream>
#include "request.h"
#include <thallium.hpp>
#include <thallium/serialization/stl/string.hpp>

namespace tl = thallium;


int main(int argc, char** argv) {

    // define the thalllium server
    tl::engine engine("tcp", THALLIUM_SERVER_MODE);

    // define the remote do_rdma procedure
    tl::remote_procedure do_rdma = engine.define("do_rdma").disable_response();

    // define the RPC method   
    std::function<void(const tl::request&, const point&)> s = 
        [&engine, &do_rdma](const tl::request &req, const point& p) {
            std::string buffer = "Matthieu";
            std::vector<std::pair<void*,std::size_t>> segments(1);
            segments[0].first  = (void*)(&buffer[0]);
            segments[0].second = buffer.size()+1;
            tl::bulk arrow_bulk = engine.expose(segments, tl::bulk_mode::read_only);
            std::cout << "About to do RDMA " << req.get_endpoint() << std::endl;
            do_rdma.on(req.get_endpoint())(arrow_bulk);
        };
    engine.define("scan", s).disable_response();

    // run the server
    std::cout << "Server running at address " << engine.self() << std::endl;
}
