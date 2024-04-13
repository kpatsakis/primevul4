void cql_server::response::write_bytes(bytes b)
{
    write_int(cast_if_fits<int32_t>(b.size()));
    _body.write(b);
}