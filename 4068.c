void cql_server::response::write_consistency(db::consistency_level c)
{
    write_short(consistency_to_wire(c));
}