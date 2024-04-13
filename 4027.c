void cql_server::connection::handle_error(future<>&& f) {
    try {
        f.get();
    } catch (const exceptions::cassandra_exception& ex) {
        try { ++_server._stats.errors[ex.code()]; } catch(...) {}
        write_response(make_error(0, ex.code(), ex.what(), tracing::trace_state_ptr()));
    } catch (std::exception& ex) {
        try { ++_server._stats.errors[exceptions::exception_code::SERVER_ERROR]; } catch(...) {}
        write_response(make_error(0, exceptions::exception_code::SERVER_ERROR, ex.what(), tracing::trace_state_ptr()));
    } catch (...) {
        try { ++_server._stats.errors[exceptions::exception_code::SERVER_ERROR]; } catch(...) {}
        write_response(make_error(0, exceptions::exception_code::SERVER_ERROR, "unknown error", tracing::trace_state_ptr()));
    }
}