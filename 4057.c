void cql_server::connection::write_response(foreign_ptr<std::unique_ptr<cql_server::response>>&& response, service_permit permit, cql_compression compression)
{
    _ready_to_respond = _ready_to_respond.then([this, compression, response = std::move(response), permit = std::move(permit)] () mutable {
        auto message = response->make_message(_version, compression);
        message.on_delete([response = std::move(response)] { });
        return _write_buf.write(std::move(message)).then([this] {
            return _write_buf.flush();
        });
    });
}