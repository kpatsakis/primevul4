void cql_server::response::write_value(std::optional<query::result_bytes_view> value)
{
    if (!value) {
        write_int(-1);
        return;
    }

    write_int(value->size_bytes());
    using boost::range::for_each;
    for_each(*value, [&] (bytes_view fragment) {
        _body.write(fragment);
    });
}