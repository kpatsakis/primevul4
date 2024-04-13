process_execute_internal(service::client_state& client_state, distributed<cql3::query_processor>& qp, request_reader in,
        uint16_t stream, cql_protocol_version_type version, cql_serialization_format serialization_format,
        service_permit permit, tracing::trace_state_ptr trace_state, bool init_trace, cql3::computed_function_values cached_pk_fn_calls) {
    cql3::prepared_cache_key_type cache_key(in.read_short_bytes());
    auto& id = cql3::prepared_cache_key_type::cql_id(cache_key);
    bool needs_authorization = false;

    // First, try to lookup in the cache of already authorized statements. If the corresponding entry is not found there
    // look for the prepared statement and then authorize it.
    auto prepared = qp.local().get_prepared(client_state.user(), cache_key);
    if (!prepared) {
        needs_authorization = true;
        prepared = qp.local().get_prepared(cache_key);
    }

    if (!prepared) {
        throw exceptions::prepared_query_not_found_exception(id);
    }

    auto q_state = std::make_unique<cql_query_state>(client_state, trace_state, std::move(permit));
    auto& query_state = q_state->query_state;
    if (version == 1) {
        std::vector<cql3::raw_value_view> values;
        in.read_value_view_list(version, values);
        auto consistency = in.read_consistency();
        q_state->options = std::make_unique<cql3::query_options>(qp.local().get_cql_config(), consistency, std::nullopt, values, false,
                                                                 cql3::query_options::specific_options::DEFAULT, serialization_format);
    } else {
        q_state->options = in.read_options(version, serialization_format, qp.local().get_cql_config());
    }
    auto& options = *q_state->options;
    if (!cached_pk_fn_calls.empty()) {
        options.set_cached_pk_function_calls(std::move(cached_pk_fn_calls));
    }
    auto skip_metadata = options.skip_metadata();

    if (init_trace) {
        tracing::set_page_size(trace_state, options.get_page_size());
        tracing::set_consistency_level(trace_state, options.get_consistency());
        tracing::set_optional_serial_consistency_level(trace_state, options.get_serial_consistency());
        tracing::add_query(trace_state, prepared->statement->raw_cql_statement);
        tracing::add_prepared_statement(trace_state, prepared);

        tracing::begin(trace_state, seastar::value_of([&id] { return seastar::format("Execute CQL3 prepared query [{}]", id); }),
                client_state.get_client_address());
    }

    auto stmt = prepared->statement;
    tracing::trace(query_state.get_trace_state(), "Checking bounds");
    if (stmt->get_bound_terms() != options.get_values_count()) {
        const auto msg = format("Invalid amount of bind variables: expected {:d} received {:d}",
                stmt->get_bound_terms(),
                options.get_values_count());
        tracing::trace(query_state.get_trace_state(), msg);
        throw exceptions::invalid_request_exception(msg);
    }

    options.prepare(prepared->bound_names);

    if (init_trace) {
        tracing::add_prepared_query_options(trace_state, options);
    }

    tracing::trace(trace_state, "Processing a statement");
    return qp.local().execute_prepared_without_checking_exception_message(std::move(prepared), std::move(cache_key), query_state, options, needs_authorization)
            .then([trace_state = query_state.get_trace_state(), skip_metadata, q_state = std::move(q_state), stream, version] (auto msg) {
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