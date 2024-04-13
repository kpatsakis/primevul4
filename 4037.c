sstring to_string(const event::topology_change::change_type t) {
    using type = event::topology_change::change_type;
    switch (t) {
    case type::NEW_NODE:     return "NEW_NODE";
    case type::REMOVED_NODE: return "REMOVED_NODE";
    case type::MOVED_NODE:   return "MOVED_NODE";
    }
    throw std::invalid_argument("unknown change type");
}