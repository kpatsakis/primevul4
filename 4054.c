cql_server::connection::process_query(uint16_t stream, request_reader in, service::client_state& client_state, service_permit permit, tracing::trace_state_ptr trace_state) {
    ++_server._stats.query_requests;
    return process(stream, in, client_state, std::move(permit), std::move(trace_state), process_query_internal);
}