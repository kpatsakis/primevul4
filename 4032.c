void cql_server::response::write_short(uint16_t n)
{
    auto u = htons(n);
    auto *s = reinterpret_cast<const int8_t*>(&u);
    _body.write(bytes_view(s, sizeof(u)));
}