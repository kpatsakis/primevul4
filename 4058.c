void cql_server::response::write_value(bytes_opt value)
{
    if (!value) {
        write_int(-1);
        return;
    }

    write_int(value->size());
    _body.write(*value);
}