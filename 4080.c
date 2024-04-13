cql_server::connection::process_on_shard(::shared_ptr<messages::result_message::bounce_to_shard> bounce_msg, uint16_t stream, fragmented_temporary_buffer::istream is,
        service::client_state& cs, service_permit permit, tracing::trace_state_ptr trace_state, Process process_fn) {
    return _server.container().invoke_on(*bounce_msg->move_to_shard(), _server._config.bounce_request_smp_service_group,
            [this, is = std::move(is), cs = cs.move_to_other_shard(), stream, permit = std::move(permit), process_fn,
             gt = tracing::global_trace_state_ptr(std::move(trace_state)),
             cached_vals = std::move(bounce_msg->take_cached_pk_function_calls())] (cql_server& server) {
        service::client_state client_state = cs.get();
        return do_with(bytes_ostream(), std::move(client_state), std::move(cached_vals),
                [this, &server, is = std::move(is), stream, process_fn,
                 trace_state = tracing::trace_state_ptr(gt)] (bytes_ostream& linearization_buffer,
                    service::client_state& client_state,
                    cql3::computed_function_values& cached_vals) mutable {
            request_reader in(is, linearization_buffer);
            return process_fn(client_state, server._query_processor, in, stream, _version, _cql_serialization_format,
                    /* FIXME */empty_service_permit(), std::move(trace_state), false, std::move(cached_vals)).then([] (auto msg) {
                // result here has to be foreign ptr
                return std::get<cql_server::result_with_foreign_response_ptr>(std::move(msg));
            });
        });
    });
}