std::unique_ptr<cql_server::response> cql_server::connection::make_mutation_write_failure_error(int16_t stream, exceptions::exception_code err, sstring msg, db::consistency_level cl, int32_t received, int32_t numfailures, int32_t blockfor, db::write_type type, const tracing::trace_state_ptr& tr_state) const
{
    if (_version < 4) {
        return make_mutation_write_timeout_error(stream, exceptions::exception_code::WRITE_TIMEOUT, std::move(msg), cl, received, blockfor, type, tr_state);
    }
    auto response = std::make_unique<cql_server::response>(stream, cql_binary_opcode::ERROR, tr_state);
    response->write_int(static_cast<int32_t>(err));
    response->write_string(msg);
    response->write_consistency(cl);
    response->write_int(received);
    response->write_int(blockfor);
    response->write_int(numfailures);
    response->write_string(format("{}", type));
    return response;
}