process_batch_internal(service::client_state& client_state, distributed<cql3::query_processor>& qp, request_reader in,
        uint16_t stream, cql_protocol_version_type version, cql_serialization_format serialization_format,
        service_permit permit, tracing::trace_state_ptr trace_state, bool init_trace, cql3::computed_function_values cached_pk_fn_calls) {
    if (version == 1) {
        throw exceptions::protocol_exception("BATCH messages are not support in version 1 of the protocol");
    }

    const auto type = in.read_byte();
    const unsigned n = in.read_short();

    std::vector<cql3::statements::batch_statement::single_statement> modifications;
    std::vector<std::vector<cql3::raw_value_view>> values;
    std::unordered_map<cql3::prepared_cache_key_type, cql3::authorized_prepared_statements_cache::value_type> pending_authorization_entries;

    modifications.reserve(n);
    values.reserve(n);

    if (init_trace) {
        tracing::begin(trace_state, "Execute batch of CQL3 queries", client_state.get_client_address());
    }

    for ([[gnu::unused]] auto i : boost::irange(0u, n)) {
        const auto kind = in.read_byte();

        std::unique_ptr<cql3::statements::prepared_statement> stmt_ptr;
        cql3::statements::prepared_statement::checked_weak_ptr ps;
        bool needs_authorization(kind == 0);

        switch (kind) {
        case 0: {
            auto query = in.read_long_string_view();
            stmt_ptr = qp.local().get_statement(query, client_state);
            ps = stmt_ptr->checked_weak_from_this();
            if (init_trace) {
                tracing::add_query(trace_state, query);
            }
            break;
        }
        case 1: {
            cql3::prepared_cache_key_type cache_key(in.read_short_bytes());
            auto& id = cql3::prepared_cache_key_type::cql_id(cache_key);

            // First, try to lookup in the cache of already authorized statements. If the corresponding entry is not found there
            // look for the prepared statement and then authorize it.
            ps = qp.local().get_prepared(client_state.user(), cache_key);
            if (!ps) {
                ps = qp.local().get_prepared(cache_key);
                if (!ps) {
                    throw exceptions::prepared_query_not_found_exception(id);
                }
                // authorize a particular prepared statement only once
                needs_authorization = pending_authorization_entries.emplace(std::move(cache_key), ps->checked_weak_from_this()).second;
            }
            if (init_trace) {
                tracing::add_query(trace_state, ps->statement->raw_cql_statement);
            }
            break;
        }
        default:
            throw exceptions::protocol_exception(
                    "Invalid query kind in BATCH messages. Must be 0 or 1 but got "
                            + std::to_string(int(kind)));
        }

        if (dynamic_cast<cql3::statements::modification_statement*>(ps->statement.get()) == nullptr) {
            throw exceptions::invalid_request_exception("Invalid statement in batch: only UPDATE, INSERT and DELETE statements are allowed.");
        }

        ::shared_ptr<cql3::statements::modification_statement> modif_statement_ptr = static_pointer_cast<cql3::statements::modification_statement>(ps->statement);
        if (init_trace) {
            tracing::add_table_name(trace_state, modif_statement_ptr->keyspace(), modif_statement_ptr->column_family());
            tracing::add_prepared_statement(trace_state, ps);
        }

        modifications.emplace_back(std::move(modif_statement_ptr), needs_authorization);

        std::vector<cql3::raw_value_view> tmp;
        in.read_value_view_list(version, tmp);

        auto stmt = ps->statement;
        if (stmt->get_bound_terms() != tmp.size()) {
            throw exceptions::invalid_request_exception(format("There were {:d} markers(?) in CQL but {:d} bound variables",
                            stmt->get_bound_terms(), tmp.size()));
        }
        values.emplace_back(std::move(tmp));
    }

    auto q_state = std::make_unique<cql_query_state>(client_state, trace_state, std::move(permit));
    auto& query_state = q_state->query_state;
    // #563. CQL v2 encodes query_options in v1 format for batch requests.
    q_state->options = std::make_unique<cql3::query_options>(cql3::query_options::make_batch_options(std::move(*in.read_options(version < 3 ? 1 : version, serialization_format,
                                                                     qp.local().get_cql_config())), std::move(values)));
    auto& options = *q_state->options;
    if (!cached_pk_fn_calls.empty()) {
        options.set_cached_pk_function_calls(std::move(cached_pk_fn_calls));
    }

    if (init_trace) {
        tracing::set_consistency_level(trace_state, options.get_consistency());
        tracing::set_optional_serial_consistency_level(trace_state, options.get_serial_consistency());
        tracing::add_prepared_query_options(trace_state, options);
        tracing::trace(trace_state, "Creating a batch statement");
    }

    auto batch = ::make_shared<cql3::statements::batch_statement>(cql3::statements::batch_statement::type(type), std::move(modifications), cql3::attributes::none(), qp.local().get_cql_stats());
    return qp.local().execute_batch_without_checking_exception_message(batch, query_state, options, std::move(pending_authorization_entries))
            .then([stream, batch, q_state = std::move(q_state), trace_state = query_state.get_trace_state(), version] (auto msg) {
        if (msg->move_to_shard()) {
            return process_fn_return_type(dynamic_pointer_cast<messages::result_message::bounce_to_shard>(msg));
        } else if (msg->is_exception()) {
            return process_fn_return_type(convert_error_message_to_coordinator_result(msg.get()));
        } else {
            tracing::trace(q_state->query_state.get_trace_state(), "Done processing - preparing a result");
            return process_fn_return_type(make_foreign(make_result(stream, *msg, trace_state, version)));
        }
    });
}