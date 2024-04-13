client_data cql_server::connection::make_client_data() const {
    client_data cd;
    std::tie(cd.ip, cd.port, cd.ct) = make_client_key(_client_state);
    cd.shard_id = this_shard_id();
    cd.protocol_version = _version;
    cd.driver_name = _client_state.get_driver_name();
    cd.driver_version = _client_state.get_driver_version();
    if (const auto user_ptr = _client_state.user(); user_ptr) {
        cd.username = user_ptr->name;
    }
    if (_ready) {
        cd.connection_stage = client_connection_stage::ready;
    } else if (_authenticating) {
        cd.connection_stage = client_connection_stage::authenticating;
    }
    return cd;
}