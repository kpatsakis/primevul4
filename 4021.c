std::unique_ptr<cql_server::response> cql_server::connection::make_supported(int16_t stream, const tracing::trace_state_ptr& tr_state) const
{
    std::multimap<sstring, sstring> opts;
    opts.insert({"CQL_VERSION", cql3::query_processor::CQL_VERSION});
    opts.insert({"COMPRESSION", "lz4"});
    opts.insert({"COMPRESSION", "snappy"});
    if (_server._config.allow_shard_aware_drivers) {
        opts.insert({"SCYLLA_SHARD", format("{:d}", this_shard_id())});
        opts.insert({"SCYLLA_NR_SHARDS", format("{:d}", smp::count)});
        opts.insert({"SCYLLA_SHARDING_ALGORITHM", dht::cpu_sharding_algorithm_name()});
        if (_server._config.shard_aware_transport_port) {
            opts.insert({"SCYLLA_SHARD_AWARE_PORT", format("{:d}", *_server._config.shard_aware_transport_port)});
        }
        if (_server._config.shard_aware_transport_port_ssl) {
            opts.insert({"SCYLLA_SHARD_AWARE_PORT_SSL", format("{:d}", *_server._config.shard_aware_transport_port_ssl)});
        }
        opts.insert({"SCYLLA_SHARDING_IGNORE_MSB", format("{:d}", _server._config.sharding_ignore_msb)});
        opts.insert({"SCYLLA_PARTITIONER", _server._config.partitioner_name});
    }
    for (cql_protocol_extension ext : supported_cql_protocol_extensions()) {
        const sstring ext_key_name = protocol_extension_name(ext);
        std::vector<sstring> params = additional_options_for_proto_ext(ext);
        if (params.empty()) {
            opts.emplace(ext_key_name, "");
        } else {
            for (sstring val : params) {
                opts.emplace(ext_key_name, std::move(val));
            }
        }
    }
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::SUPPORTED, tr_state);
    response->write_string_multimap(std::move(opts));
    return response;
}