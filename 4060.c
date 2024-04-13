std::unique_ptr<cql_server::response> cql_server::connection::make_auth_success(int16_t stream, bytes b, const tracing::trace_state_ptr& tr_state) const {
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::AUTH_SUCCESS, tr_state);
    response->write_bytes(std::move(b));
    return response;
}