cql_server::connection::connection(cql_server& server, socket_address server_addr, connected_socket&& fd, socket_address addr)
    : generic_server::connection{server, std::move(fd)}
    , _server(server)
    , _server_addr(server_addr)
    , _client_state(service::client_state::external_tag{}, server._auth_service, &server._sl_controller, server.timeout_config(), addr)
{
    _shedding_timer.set_callback([this] {
        clogger.debug("Shedding all incoming requests due to overload");
        _shed_incoming_requests = true;
    });
}