cql_server::connection::frame_size() const {
    if (_version < 3) {
        return 8;
    } else {
        return 9;
    }
}