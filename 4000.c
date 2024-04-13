future<std::unique_ptr<cql_server::response>> cql_server::connection::process_auth_response(uint16_t stream, request_reader in, service::client_state& client_state,
        tracing::trace_state_ptr trace_state) {
    ++_server._stats.auth_responses;
    auto sasl_challenge = client_state.get_auth_service()->underlying_authenticator().new_sasl_challenge();
    auto buf = in.read_raw_bytes_view(in.bytes_left());
    auto challenge = sasl_challenge->evaluate_response(buf);
    if (sasl_challenge->is_complete()) {
        return sasl_challenge->get_authenticated_user().then([this, sasl_challenge, stream, &client_state, challenge = std::move(challenge), trace_state](auth::authenticated_user user) mutable {
            client_state.set_login(std::move(user));
            auto f = client_state.check_user_can_login();
            f = f.then([&client_state] {
                return client_state.maybe_update_per_service_level_params();
            });
            return f.then([this, stream, &client_state, challenge = std::move(challenge), trace_state]() mutable {
                return make_ready_future<std::unique_ptr<cql_server::response>>(make_auth_success(stream, std::move(challenge), trace_state));
            });
        });
    }
    return make_ready_future<std::unique_ptr<cql_server::response>>(make_auth_challenge(stream, std::move(challenge), trace_state));
}