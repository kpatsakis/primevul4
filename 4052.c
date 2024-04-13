cql_server::advertise_new_connection(shared_ptr<generic_server::connection> raw_conn) {
    if (auto conn = dynamic_pointer_cast<connection>(raw_conn)) {
        const auto ip = conn->get_client_state().get_client_address().addr();
        const auto port = conn->get_client_state().get_client_port();
        clogger.trace("Advertising new connection from CQL client {}:{}", ip, port);
    }
    return make_ready_future<>();
}