void cql_server::response::write_bytes_as_string(bytes_view s)
{
    write_short(cast_if_fits<uint16_t>(s.size()));
    _body.write(s);
}