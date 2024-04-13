cql_server::connection::make_schema_change_event(const event::schema_change& event) const
{
    auto response = std::make_unique<cql_server::response>(-1, cql_binary_opcode::EVENT, tracing::trace_state_ptr());
    response->write_string("SCHEMA_CHANGE");
    response->serialize(event, _version);
    return response;
}