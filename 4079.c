cql_server::connection::make_topology_change_event(const event::topology_change& event) const
{
    auto response = std::make_unique<cql_server::response>(-1, cql_binary_opcode::EVENT, tracing::trace_state_ptr());
    response->write_string("TOPOLOGY_CHANGE");
    response->write_string(to_string(event.change));
    response->write_inet(event.node);
    return response;
}