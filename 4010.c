void cql_server::response::write_string_map(std::map<sstring, sstring> string_map)
{
    write_short(cast_if_fits<uint16_t>(string_map.size()));
    for (auto&& s : string_map) {
        write_string(s.first);
        write_string(s.second);
    }
}