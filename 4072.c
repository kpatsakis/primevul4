cql_server::make_connection(socket_address server_addr, connected_socket&& fd, socket_address addr) {
    auto conn = make_shared<connection>(*this, server_addr, std::move(fd), std::move(addr));
    ++_stats.connects;
    ++_stats.connections;
    return conn;
}