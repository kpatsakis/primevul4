future<std::unique_ptr<cql_server::response>> cql_server::connection::process_options(uint16_t stream, request_reader in, service::client_state& client_state,
        tracing::trace_state_ptr trace_state) {
    ++_server._stats.options_requests;
    return make_ready_future<std::unique_ptr<cql_server::response>>(make_supported(stream, std::move(trace_state)));
}