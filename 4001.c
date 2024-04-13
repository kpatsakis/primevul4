std::unique_ptr<cql_server::response> cql_server::connection::make_autheticate(int16_t stream, std::string_view clz, const tracing::trace_state_ptr& tr_state) const
{
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::AUTHENTICATE, tr_state);
    response->write_string(clz);
    return response;
}