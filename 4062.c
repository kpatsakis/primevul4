std::unique_ptr<cql_server::response> cql_server::connection::make_function_failure_error(int16_t stream, exceptions::exception_code err, sstring msg, sstring ks_name, sstring func_name, std::vector<sstring> args, const tracing::trace_state_ptr& tr_state) const
{
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::ERROR, tr_state);
    response->write_int(static_cast<int32_t>(err));
    response->write_string(msg);
    response->write_string(ks_name);
    response->write_string(func_name);
    response->write_string_list(args);
    return response;
}