scattered_message<char> cql_server::response::make_message(uint8_t version, cql_compression compression) {
    if (compression != cql_compression::none) {
        compress(compression);
    }
    scattered_message<char> msg;
    auto frame = make_frame(version, _body.size());
    msg.append(std::move(frame));
    for (auto&& fragment : _body.fragments()) {
        msg.append_static(reinterpret_cast<const char*>(fragment.data()), fragment.size());
    }
    return msg;
}