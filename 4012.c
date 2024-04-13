sstring to_string(const event::schema_change::change_type t) {
    switch (t) {
    case event::schema_change::change_type::CREATED: return "CREATED";
    case event::schema_change::change_type::UPDATED: return "UPDATED";
    case event::schema_change::change_type::DROPPED: return "DROPPED";
    }
    assert(false && "unreachable");
}