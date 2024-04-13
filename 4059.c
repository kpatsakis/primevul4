sstring to_string(const event::schema_change::target_type t) {
    switch (t) {
    case event::schema_change::target_type::KEYSPACE: return "KEYSPACE";
    case event::schema_change::target_type::TABLE:    return "TABLE";
    case event::schema_change::target_type::TYPE:     return "TYPE";
    case event::schema_change::target_type::FUNCTION: return "FUNCTION";
    case event::schema_change::target_type::AGGREGATE:return "AGGREGATE";
    }
    assert(false && "unreachable");
}