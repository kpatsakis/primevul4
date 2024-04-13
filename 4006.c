std::unique_ptr<cql_server::response> cql_server::connection::make_unprepared_error(int16_t stream, exceptions::exception_code err, sstring msg, bytes id, const tracing::trace_state_ptr& tr_state) const
{
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::ERROR, tr_state);
    response->write_int(static_cast<int32_t>(err));
    response->write_string(msg);
    response->write_short_bytes(id);
    return response;
}