void cql_server::response::write_short_bytes(bytes b)
{
    write_short(cast_if_fits<uint16_t>(b.size()));
    _body.write(b);
}