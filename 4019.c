void cql_server::response::write_string(std::string_view s)
{
    write_short(cast_if_fits<uint16_t>(s.size()));
    _body.write(bytes_view(reinterpret_cast<const int8_t*>(s.data()), s.size()));
}