void cql_server::response::write(const cql3::metadata& m, bool no_metadata) {
    auto flags = m.flags();
    bool global_tables_spec = m.flags().contains<cql3::metadata::flag::GLOBAL_TABLES_SPEC>();
    bool has_more_pages = m.flags().contains<cql3::metadata::flag::HAS_MORE_PAGES>();

    if (no_metadata) {
        flags.set<cql3::metadata::flag::NO_METADATA>();
    }

    write_int(flags.mask());
    write_int(m.column_count());

    if (has_more_pages) {
        write_value(m.paging_state()->serialize());
    }

    if (no_metadata) {
        return;
    }

    auto names_i = m.get_names().begin();

    if (global_tables_spec) {
        auto first_spec = *names_i;
        write_string(first_spec->ks_name);
        write_string(first_spec->cf_name);
    }

    for (uint32_t i = 0; i < m.column_count(); ++i, ++names_i) {
        lw_shared_ptr<cql3::column_specification> name = *names_i;
        if (!global_tables_spec) {
            write_string(name->ks_name);
            write_string(name->cf_name);
        }
        write_string(name->name->text());
        type_codec::encode(*this, name->type);
    }
}