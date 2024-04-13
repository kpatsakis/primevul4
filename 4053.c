make_result(int16_t stream, messages::result_message& msg, const tracing::trace_state_ptr& tr_state,
        cql_protocol_version_type version, bool skip_metadata) {
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::RESULT, tr_state);
    if (__builtin_expect(!msg.warnings().empty() && version > 3, false)) {
        response->set_frame_flag(cql_frame_flags::warning);
        response->write_string_list(msg.warnings());
    }
    cql_server::fmt_visitor fmt{version, *response, skip_metadata};
    msg.accept(fmt);
    return response;
}