cql_server::cql_server(distributed<cql3::query_processor>& qp, auth::service& auth_service,
        service::memory_limiter& ml, cql_server_config config, const db::config& db_cfg,
        qos::service_level_controller& sl_controller, gms::gossiper& g)
    : server("CQLServer", clogger)
    , _query_processor(qp)
    , _config(config)
    , _max_request_size(config.max_request_size)
    , _max_concurrent_requests(db_cfg.max_concurrent_requests_per_shard)
    , _memory_available(ml.get_semaphore())
    , _notifier(std::make_unique<event_notifier>(*this))
    , _auth_service(auth_service)
    , _sl_controller(sl_controller)
    , _gossiper(g)
{
    namespace sm = seastar::metrics;

    auto ls = {
        sm::make_counter("startups", _stats.startups,
                        sm::description("Counts the total number of received CQL STARTUP messages.")),

        sm::make_counter("auth_responses", _stats.auth_responses,
                        sm::description("Counts the total number of received CQL AUTH messages.")),
        
        sm::make_counter("options_requests", _stats.options_requests,
                        sm::description("Counts the total number of received CQL OPTIONS messages.")),

        sm::make_counter("query_requests", _stats.query_requests,
                        sm::description("Counts the total number of received CQL QUERY messages.")),

        sm::make_counter("prepare_requests", _stats.prepare_requests,
                        sm::description("Counts the total number of received CQL PREPARE messages.")),

        sm::make_counter("execute_requests", _stats.execute_requests,
                        sm::description("Counts the total number of received CQL EXECUTE messages.")),

        sm::make_counter("batch_requests", _stats.batch_requests,
                        sm::description("Counts the total number of received CQL BATCH messages.")),

        sm::make_counter("register_requests", _stats.register_requests,
                        sm::description("Counts the total number of received CQL REGISTER messages.")),

        sm::make_counter("cql-connections", _stats.connects,
                        sm::description("Counts a number of client connections.")),

        sm::make_gauge("current_connections", _stats.connections,
                        sm::description("Holds a current number of client connections.")),

        sm::make_counter("requests_served", _stats.requests_served,
                        sm::description("Counts a number of served requests.")),

        sm::make_gauge("requests_serving", _stats.requests_serving,
                        sm::description("Holds a number of requests that are being processed right now.")),

        sm::make_gauge("requests_blocked_memory_current", [this] { return _memory_available.waiters(); },
                        sm::description(
                            seastar::format("Holds the number of requests that are currently blocked due to reaching the memory quota limit ({}B). "
                                            "Non-zero value indicates that our bottleneck is memory and more specifically - the memory quota allocated for the \"CQL transport\" component.", _max_request_size))),
        sm::make_counter("requests_blocked_memory", _stats.requests_blocked_memory,
                        sm::description(
                            seastar::format("Holds an incrementing counter with the requests that ever blocked due to reaching the memory quota limit ({}B). "
                                            "The first derivative of this value shows how often we block due to memory exhaustion in the \"CQL transport\" component.", _max_request_size))),
        sm::make_counter("requests_shed", _stats.requests_shed,
                        sm::description("Holds an incrementing counter with the requests that were shed due to overload (threshold configured via max_concurrent_requests_per_shard). "
                                            "The first derivative of this value shows how often we shed requests due to overload in the \"CQL transport\" component.")),
        sm::make_gauge("requests_memory_available", [this] { return _memory_available.current(); },
                        sm::description(
                            seastar::format("Holds the amount of available memory for admitting new requests (max is {}B)."
                                            "Zero value indicates that our bottleneck is memory and more specifically - the memory quota allocated for the \"CQL transport\" component.", _max_request_size)))
    };

    std::vector<sm::metric_definition> transport_metrics;
    for (auto& m : ls) {
        transport_metrics.emplace_back(std::move(m));
    }

    sm::label cql_error_label("type");
    for (const auto& e : exceptions::exception_map()) {
        _stats.errors.insert({e.first, 0});
        auto label_instance = cql_error_label(e.second);

        transport_metrics.emplace_back(
            sm::make_counter("cql_errors_total", sm::description("Counts the total number of returned CQL errors."),
                        {label_instance},
                        [this, code = e.first] { auto it = _stats.errors.find(code); return it != _stats.errors.end() ? it->second : 0; })
        );
    }

    _metrics.add_group("transport", std::move(transport_metrics));
}