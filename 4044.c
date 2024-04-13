std::unique_ptr<cql_server::response> cql_server::connection::make_ready(int16_t stream, const tracing::trace_state_ptr& tr_state) const
{
    return std::make_unique<cql_server::response>(stream, cql_binary_opcode::READY, tr_state);
}