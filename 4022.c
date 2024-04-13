void cql_server::response::write_string_list(std::vector<sstring> string_list)
{
    write_short(cast_if_fits<uint16_t>(string_list.size()));
    for (auto&& s : string_list) {
        write_string(s);
    }
}