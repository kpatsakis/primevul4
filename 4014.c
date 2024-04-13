cql_server::connection::process_register(uint16_t stream, request_reader in, service::client_state& client_state,
        tracing::trace_state_ptr trace_state) {
    ++_server._stats.register_requests;
    std::vector<sstring> event_types;
    in.read_string_list(event_types);
    for (auto&& event_type : event_types) {
        auto et = parse_event_type(event_type);
        _server._notifier->register_event(et, this);
    }
    _ready = true;
    return make_ready_future<std::unique_ptr<cql_server::response>>(make_ready(stream, std::move(trace_state)));
}