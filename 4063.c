void cql_server::response::write_inet(socket_address inet)
{
    auto addr = inet.addr();
    write_byte(uint8_t(addr.size()));
    auto * p = static_cast<const int8_t*>(addr.data());
    _body.write(bytes_view(p, addr.size()));
    write_int(inet.port());
}