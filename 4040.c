future<utils::chunked_vector<client_data>> cql_server::get_client_data() {
    utils::chunked_vector<client_data> ret;
    co_await for_each_gently([&ret] (const generic_server::connection& c) {
        const connection& conn = dynamic_cast<const connection&>(c);
        ret.emplace_back(conn.make_client_data());
    });
    co_return ret;
}