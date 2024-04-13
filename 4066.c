void cql_server::response::write_long_string(const sstring& s)
{
    write_int(cast_if_fits<int32_t>(s.size()));
    _body.write(bytes_view(reinterpret_cast<const int8_t*>(s.data()), s.size()));
}