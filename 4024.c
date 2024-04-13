sstring to_string(const event::status_change::status_type t) {
    using type = event::status_change::status_type;
    switch (t) {
    case type::UP:   return "UP";
    case type::DOWN: return "DOWN";
    }
    throw std::invalid_argument("unknown change type");
}