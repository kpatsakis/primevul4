cql_server::connection::make_status_change_event(const event::status_change& event) const
{
    auto response = std::make_unique<cql_server::response>(-1, cql_binary_opcode::EVENT, tracing::trace_state_ptr());
    response->write_string("STATUS_CHANGE");
    response->write_string(to_string(event.status));
    response->write_inet(event.node);
    return response;
}