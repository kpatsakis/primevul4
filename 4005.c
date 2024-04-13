cql_server::connection::parse_frame(temporary_buffer<char> buf) const {
    if (buf.size() != frame_size()) {
        throw cql_frame_error();
    }
    cql_binary_frame_v3 v3;
    switch (_version) {
    case 1:
    case 2: {
        cql_binary_frame_v1 raw = read_unaligned<cql_binary_frame_v1>(buf.get());
        auto cooked = net::ntoh(raw);
        v3.version = cooked.version;
        v3.flags = cooked.flags;
        v3.opcode = cooked.opcode;
        v3.stream = cooked.stream;
        v3.length = cooked.length;
        break;
    }
    case 3:
    case 4: {
        cql_binary_frame_v3 raw = read_unaligned<cql_binary_frame_v3>(buf.get());
        v3 = net::ntoh(raw);
        break;
    }
    default:
        throw exceptions::protocol_exception(format("Invalid or unsupported protocol version: {:d}", _version));
    }
    if (v3.version != _version) {
        throw exceptions::protocol_exception(format("Invalid message version. Got {:d} but previous messages on this connection had version {:d}", v3.version, _version));

    }
    return v3;
}