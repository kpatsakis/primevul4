    cql_server::connection::process_request_one(fragmented_temporary_buffer::istream fbuf, uint8_t op, uint16_t stream, service::client_state& client_state, tracing_request_type tracing_request, service_permit permit) {
    using auth_state = service::client_state::auth_state;

    auto cqlop = static_cast<cql_binary_opcode>(op);
    tracing::trace_state_props_set trace_props;

    trace_props.set_if<tracing::trace_state_props::log_slow_query>(tracing::tracing::get_local_tracing_instance().slow_query_tracing_enabled());
    trace_props.set_if<tracing::trace_state_props::full_tracing>(tracing_request != tracing_request_type::not_requested);
    tracing::trace_state_ptr trace_state;

    if (trace_props) {
        if (cqlop == cql_binary_opcode::QUERY ||
            cqlop == cql_binary_opcode::PREPARE ||
            cqlop == cql_binary_opcode::EXECUTE ||
            cqlop == cql_binary_opcode::BATCH) {
            trace_props.set_if<tracing::trace_state_props::write_on_close>(tracing_request == tracing_request_type::write_on_close);
            trace_state = tracing::tracing::get_local_tracing_instance().create_session(tracing::trace_type::QUERY, trace_props);
        }
    }

    tracing::set_request_size(trace_state, fbuf.bytes_left());

    auto linearization_buffer = std::make_unique<bytes_ostream>();
    auto linearization_buffer_ptr = linearization_buffer.get();
    return futurize_invoke([this, cqlop, stream, &fbuf, &client_state, linearization_buffer_ptr, permit = std::move(permit), trace_state] () mutable {
        // When using authentication, we need to ensure we are doing proper state transitions,
        // i.e. we cannot simply accept any query/exec ops unless auth is complete
        switch (client_state.get_auth_state()) {
            case auth_state::UNINITIALIZED:
                if (cqlop != cql_binary_opcode::STARTUP && cqlop != cql_binary_opcode::OPTIONS) {
                    throw exceptions::protocol_exception(format("Unexpected message {:d}, expecting STARTUP or OPTIONS", int(cqlop)));
                }
                break;
            case auth_state::AUTHENTICATION:
                // Support both SASL auth from protocol v2 and the older style Credentials auth from v1
                if (cqlop != cql_binary_opcode::AUTH_RESPONSE && cqlop != cql_binary_opcode::CREDENTIALS) {
                    throw exceptions::protocol_exception(format("Unexpected message {:d}, expecting {}", int(cqlop), _version == 1 ? "CREDENTIALS" : "SASL_RESPONSE"));
                }
                break;
            case auth_state::READY: default:
                if (cqlop == cql_binary_opcode::STARTUP) {
                    throw exceptions::protocol_exception("Unexpected message STARTUP, the connection is already initialized");
                }
                break;
        }

        tracing::set_username(trace_state, client_state.user());

        auto wrap_in_foreign = [] (future<std::unique_ptr<cql_server::response>> f) {
            return f.then([] (std::unique_ptr<cql_server::response> p) {
                return make_ready_future<result_with_foreign_response_ptr>(make_foreign(std::move(p)));
            });
        };
        auto in = request_reader(std::move(fbuf), *linearization_buffer_ptr);
        switch (cqlop) {
        case cql_binary_opcode::STARTUP:       return wrap_in_foreign(process_startup(stream, std::move(in), client_state, trace_state));
        case cql_binary_opcode::AUTH_RESPONSE: return wrap_in_foreign(process_auth_response(stream, std::move(in), client_state, trace_state));
        case cql_binary_opcode::OPTIONS:       return wrap_in_foreign(process_options(stream, std::move(in), client_state, trace_state));
        case cql_binary_opcode::QUERY:         return process_query(stream, std::move(in), client_state, std::move(permit), trace_state);
        case cql_binary_opcode::PREPARE:       return wrap_in_foreign(process_prepare(stream, std::move(in), client_state, trace_state));
        case cql_binary_opcode::EXECUTE:       return process_execute(stream, std::move(in), client_state, std::move(permit), trace_state);
        case cql_binary_opcode::BATCH:         return process_batch(stream, std::move(in), client_state, std::move(permit), trace_state);
        case cql_binary_opcode::REGISTER:      return wrap_in_foreign(process_register(stream, std::move(in), client_state, trace_state));
        default:                               throw exceptions::protocol_exception(format("Unknown opcode {:d}", int(cqlop)));
        }
    }).then_wrapped([this, cqlop, stream, &client_state, linearization_buffer = std::move(linearization_buffer), trace_state] (future<result_with_foreign_response_ptr> f) {
        auto stop_trace = defer([&] {
            tracing::stop_foreground(trace_state);
        });
        --_server._stats.requests_serving;

        return utils::result_into_future<result_with_foreign_response_ptr>(utils::result_try([&] () -> result_with_foreign_response_ptr {
            result_with_foreign_response_ptr res = f.get0();
            if (!res) {
                return res;
            }

            auto response = std::move(res).assume_value();
            auto res_op = response->opcode();

            // and modify state now that we've generated a response.
            switch (client_state.get_auth_state()) {
            case auth_state::UNINITIALIZED:
                if (cqlop == cql_binary_opcode::STARTUP) {
                    if (res_op == cql_binary_opcode::AUTHENTICATE) {
                        client_state.set_auth_state(auth_state::AUTHENTICATION);
                    } else if (res_op == cql_binary_opcode::READY) {
                        client_state.set_auth_state(auth_state::READY);
                    }
                }
                break;
            case auth_state::AUTHENTICATION:
                // Support both SASL auth from protocol v2 and the older style Credentials auth from v1
                if (cqlop != cql_binary_opcode::AUTH_RESPONSE && cqlop != cql_binary_opcode::CREDENTIALS) {
                    throw exceptions::protocol_exception(format("Unexpected message {:d}, expecting AUTH_RESPONSE or CREDENTIALS", int(cqlop)));
                }
                if (res_op == cql_binary_opcode::READY || res_op == cql_binary_opcode::AUTH_SUCCESS) {
                    client_state.set_auth_state(auth_state::READY);
                }
                break;
            default:
            case auth_state::READY:
                break;
            }

            tracing::set_response_size(trace_state, response->size());
            return response;
        },  utils::result_catch<exceptions::unavailable_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_unavailable_error(stream, ex.code(), ex.what(), ex.consistency, ex.required, ex.alive, trace_state);
        }), utils::result_catch<exceptions::read_timeout_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_read_timeout_error(stream, ex.code(), ex.what(), ex.consistency, ex.received, ex.block_for, ex.data_present, trace_state);
        }), utils::result_catch<exceptions::read_failure_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_read_failure_error(stream, ex.code(), ex.what(), ex.consistency, ex.received, ex.failures, ex.block_for, ex.data_present, trace_state);
        }), utils::result_catch<exceptions::mutation_write_timeout_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_mutation_write_timeout_error(stream, ex.code(), ex.what(), ex.consistency, ex.received, ex.block_for, ex.type, trace_state);
        }), utils::result_catch<exceptions::mutation_write_failure_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_mutation_write_failure_error(stream, ex.code(), ex.what(), ex.consistency, ex.received, ex.failures, ex.block_for, ex.type, trace_state);
        }), utils::result_catch<exceptions::already_exists_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_already_exists_error(stream, ex.code(), ex.what(), ex.ks_name, ex.cf_name, trace_state);
        }), utils::result_catch<exceptions::prepared_query_not_found_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_unprepared_error(stream, ex.code(), ex.what(), ex.id, trace_state);
        }), utils::result_catch<exceptions::function_execution_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_function_failure_error(stream, ex.code(), ex.what(), ex.ks_name, ex.func_name, ex.args, trace_state);
        }), utils::result_catch<exceptions::rate_limit_exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_rate_limit_error(stream, ex.code(), ex.what(), ex.op_type, ex.rejected_by_coordinator, trace_state, client_state);
        }), utils::result_catch<exceptions::cassandra_exception>([&] (const auto& ex) {
            // Note: the CQL protocol specifies that many types of errors have
            // mandatory parameters. These cassandra_exception subclasses MUST
            // be handled above. This default "cassandra_exception" case is
            // only appropriate for the specific types of errors which do not have
            // additional information, such as invalid_request_exception.
            // TODO: consider listing those types explicitly, instead of the
            // catch-all type cassandra_exception.
            try { ++_server._stats.errors[ex.code()]; } catch(...) {}
            return make_error(stream, ex.code(), ex.what(), trace_state);
        }), utils::result_catch<std::exception>([&] (const auto& ex) {
            try { ++_server._stats.errors[exceptions::exception_code::SERVER_ERROR]; } catch(...) {}
            sstring msg = ex.what();
            try {
                std::rethrow_if_nested(ex);
            } catch (...) {
                std::ostringstream ss;
                ss << msg << ": " << std::current_exception();
                msg = ss.str();
            }
            return make_error(stream, exceptions::exception_code::SERVER_ERROR, msg, trace_state);
        }), utils::result_catch_dots([&] () {
            try { ++_server._stats.errors[exceptions::exception_code::SERVER_ERROR]; } catch(...) {}
            return make_error(stream, exceptions::exception_code::SERVER_ERROR, "unknown error", trace_state);
        })));
    });
}