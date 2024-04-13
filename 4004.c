future<std::unique_ptr<cql_server::response>> cql_server::connection::process_prepare(uint16_t stream, request_reader in, service::client_state& client_state,
        tracing::trace_state_ptr trace_state) {
    ++_server._stats.prepare_requests;

    auto query = sstring(in.read_long_string_view());

    tracing::add_query(trace_state, query);
    tracing::begin(trace_state, "Preparing CQL3 query", client_state.get_client_address());

    auto cpu_id = this_shard_id();
    auto cpus = boost::irange(0u, smp::count);
    return parallel_for_each(cpus.begin(), cpus.end(), [this, query, cpu_id, &client_state] (unsigned int c) mutable {
        if (c != cpu_id) {
            return smp::submit_to(c, [this, query, &client_state] () mutable {
                return _server._query_processor.local().prepare(std::move(query), client_state, false).discard_result();
            });
        } else {
            return make_ready_future<>();
        }
    }).then([this, query, stream, &client_state, trace_state] () mutable {
        tracing::trace(trace_state, "Done preparing on remote shards");
        return _server._query_processor.local().prepare(std::move(query), client_state, false).then([this, stream, &client_state, trace_state] (auto msg) {
            tracing::trace(trace_state, "Done preparing on a local shard - preparing a result. ID is [{}]", seastar::value_of([&msg] {
                return messages::result_message::prepared::cql::get_id(msg);
            }));
            return make_result(stream, *msg, trace_state, _version);
        });
    });
}