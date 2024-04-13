process_query_internal(service::client_state& client_state, distributed<cql3::query_processor>& qp, request_reader in,
        uint16_t stream, cql_protocol_version_type version, cql_serialization_format serialization_format,
        service_permit permit, tracing::trace_state_ptr trace_state, bool init_trace, cql3::computed_function_values cached_pk_fn_calls) {
    auto query = in.read_long_string_view();
    auto q_state = std::make_unique<cql_query_state>(client_state, trace_state, std::move(permit));
    auto& query_state = q_state->query_state;
    q_state->options = in.read_options(version, serialization_format, qp.local().get_cql_config());
    auto& options = *q_state->options;
    if (!cached_pk_fn_calls.empty()) {
        options.set_cached_pk_function_calls(std::move(cached_pk_fn_calls));
    }
    auto skip_metadata = options.skip_metadata();

    if (init_trace) {
        tracing::set_page_size(trace_state, options.get_page_size());
        tracing::set_consistency_level(trace_state, options.get_consistency());
        tracing::set_optional_serial_consistency_level(trace_state, options.get_serial_consistency());
        tracing::add_query(trace_state, query);
        tracing::set_user_timestamp(trace_state, options.get_specific_options().timestamp);

        tracing::begin(trace_state, "Execute CQL3 query", client_state.get_client_address());
    }

    return qp.local().execute_direct_without_checking_exception_message(query, query_state, options).then([q_state = std::move(q_state), stream, skip_metadata, version] (auto msg) {
        if (msg->move_to_shard()) {
            return process_fn_return_type(dynamic_pointer_cast<messages::result_message::bounce_to_shard>(msg));
        } else if (msg->is_exception()) {
            return process_fn_return_type(convert_error_message_to_coordinator_result(msg.get()));
        } else {
            tracing::trace(q_state->query_state.get_trace_state(), "Done processing - preparing a result");
            return process_fn_return_type(make_foreign(make_result(stream, *msg, q_state->query_state.get_trace_state(), version, skip_metadata)));
        }
    });
}