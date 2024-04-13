std::unique_ptr<cql_server::response> cql_server::connection::make_rate_limit_error(int16_t stream, exceptions::exception_code err, sstring msg, db::operation_type op_type, bool rejected_by_coordinator, const tracing::trace_state_ptr& tr_state, const service::client_state& client_state) const
{
    if (!client_state.is_protocol_extension_set(cql_protocol_extension::RATE_LIMIT_ERROR)) {
        return make_error(stream, exceptions::exception_code::CONFIG_ERROR, std::move(msg), tr_state);
    }

    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::ERROR, tr_state);
    response->write_int(static_cast<int32_t>(err));
    response->write_string(msg);
    response->write_byte(static_cast<uint8_t>(op_type));
    response->write_byte(static_cast<uint8_t>(rejected_by_coordinator));
    return response;
}