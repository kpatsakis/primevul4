future<std::unique_ptr<cql_server::response>> cql_server::connection::process_startup(uint16_t stream, request_reader in, service::client_state& client_state,
        tracing::trace_state_ptr trace_state) {
    ++_server._stats.startups;
    auto options = in.read_string_map();
    auto compression_opt = options.find("COMPRESSION");
    if (compression_opt != options.end()) {
         auto compression = compression_opt->second;
         std::transform(compression.begin(), compression.end(), compression.begin(), ::tolower);
         if (compression == "lz4") {
             _compression = cql_compression::lz4;
         } else if (compression == "snappy") {
             _compression = cql_compression::snappy;
         } else {
             throw exceptions::protocol_exception(format("Unknown compression algorithm: {}", compression));
         }
    }

    if (auto driver_ver_opt = options.find("DRIVER_VERSION"); driver_ver_opt != options.end()) {
        _client_state.set_driver_version(driver_ver_opt->second);
    }
    if (auto driver_name_opt = options.find("DRIVER_NAME"); driver_name_opt != options.end()) {
        _client_state.set_driver_name(driver_name_opt->second);
    }

    cql_protocol_extension_enum_set cql_proto_exts;
    for (cql_protocol_extension ext : supported_cql_protocol_extensions()) {
        if (options.contains(protocol_extension_name(ext))) {
            cql_proto_exts.set(ext);
        }
    }
    _client_state.set_protocol_extensions(std::move(cql_proto_exts));
    std::unique_ptr<cql_server::response> res;
    if (auto& a = client_state.get_auth_service()->underlying_authenticator(); a.require_authentication()) {
        _authenticating = true;
        res = make_autheticate(stream, a.qualified_java_name(), trace_state);
    } else {
        _ready = true;
        res = make_ready(stream, trace_state);
    }

    return make_ready_future<decltype(res)>(std::move(res));
}