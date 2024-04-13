void cql_server::response::write_string_multimap(std::multimap<sstring, sstring> string_map)
{
    std::vector<sstring> keys;
    for (auto it = string_map.begin(), end = string_map.end(); it != end; it = string_map.upper_bound(it->first)) {
        keys.push_back(it->first);
    }
    write_short(cast_if_fits<uint16_t>(keys.size()));
    for (auto&& key : keys) {
        std::vector<sstring> values;
        auto range = string_map.equal_range(key);
        for (auto it = range.first; it != range.second; ++it) {
            values.push_back(it->second);
        }
        write_string(key);
        write_string_list(values);
    }
}