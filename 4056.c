cql_server::connection::process(uint16_t stream, request_reader in, service::client_state& client_state, service_permit permit,
        tracing::trace_state_ptr trace_state, Process process_fn) {
    fragmented_temporary_buffer::istream is = in.get_stream();

    return process_fn(client_state, _server._query_processor, in, stream,
            _version, _cql_serialization_format, permit, trace_state, true, {})
            .then([stream, &client_state, this, is, permit, process_fn, trace_state]
                   (process_fn_return_type msg) mutable {
        auto* bounce_msg = std::get_if<shared_ptr<messages::result_message::bounce_to_shard>>(&msg);
        if (bounce_msg) {
            return process_on_shard(*bounce_msg, stream, is, client_state, std::move(permit), trace_state, process_fn);
        }
        auto ptr = std::get<cql_server::result_with_foreign_response_ptr>(std::move(msg));
        return make_ready_future<cql_server::result_with_foreign_response_ptr>(std::move(ptr));
    });
}