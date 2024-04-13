cql_server::connection::read_frame() {
    using ret_type = std::optional<cql_binary_frame_v3>;
    if (!_version) {
        // We don't know the frame size before reading the first frame,
        // so read just one byte, and then read the rest of the frame.
        return _read_buf.read_exactly(1).then([this] (temporary_buffer<char> buf) {
            if (buf.empty()) {
                return make_ready_future<ret_type>();
            }
            _version = buf[0];
            init_cql_serialization_format();
            if (_version < 1 || _version > current_version) {
                auto client_version = _version;
                _version = current_version;
                throw exceptions::protocol_exception(format("Invalid or unsupported protocol version: {:d}", client_version));
            }


            return _read_buf.read_exactly(frame_size() - 1).then([this] (temporary_buffer<char> tail) {
                temporary_buffer<char> full(frame_size());
                full.get_write()[0] = _version;
                std::copy(tail.get(), tail.get() + tail.size(), full.get_write() + 1);
                auto frame = parse_frame(std::move(full));
                // This is the very first frame, so reject obviously incorrect frames, to
                // avoid allocating large amounts of memory for the message body
                if (frame.length > 100'000) {
                    // The STARTUP message body is a [string map] containing just a few options,
                    // so it should be smaller that 100kB. See #4366.
                    throw exceptions::protocol_exception(format("Initial message size too large ({:d}), rejecting as invalid", frame.length));
                }
                return make_ready_future<ret_type>(frame);
            });
        });
    } else {
        // Not the first frame, so we know the size.
        return _read_buf.read_exactly(frame_size()).then([this] (temporary_buffer<char> buf) {
            if (buf.empty()) {
                return make_ready_future<ret_type>();
            }
            return make_ready_future<ret_type>(parse_frame(std::move(buf)));
        });
    }
}