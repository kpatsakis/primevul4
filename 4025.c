void cql_server::response::write_byte(uint8_t b)
{
    auto s = reinterpret_cast<const int8_t*>(&b);
    _body.write(bytes_view(s, sizeof(b)));
}