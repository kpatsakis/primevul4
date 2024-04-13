void cql_server::response::write(const cql3::prepared_metadata& m, uint8_t version)
{
    bool global_tables_spec = m.flags().contains<cql3::prepared_metadata::flag::GLOBAL_TABLES_SPEC>();

    write_int(m.flags().mask());
    write_int(m.names().size());

    if (version >= 4) {
        if (!global_tables_spec) {
            write_int(0);
        } else {
            write_int(m.partition_key_bind_indices().size());
            for (uint16_t bind_index : m.partition_key_bind_indices()) {
                write_short(bind_index);
            }
        }
    }

    if (global_tables_spec) {
        write_string(m.names()[0]->ks_name);
        write_string(m.names()[0]->cf_name);
    }

    for (auto const& name : m.names()) {
        if (!global_tables_spec) {
            write_string(name->ks_name);
            write_string(name->cf_name);
        }
        write_string(name->name->text());
        type_codec::encode(*this, name->type);
    }
}