cql_server::response::placeholder<int32_t> cql_server::response::write_int_placeholder() {
    return placeholder<int32_t>(_body.write_place_holder(sizeof(int32_t)));
}