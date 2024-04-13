    static void encode(cql_server::response& r, data_type type) {
        type = type->underlying_type();

        // For compatibility sake, we still return DateType as the timestamp type in resultSet metadata (#5723)
        if (type == date_type) {
            type = timestamp_type;
        }

        auto i = type_id_to_type.find(type);
        if (i != type_id_to_type.end()) {
            r.write_short(static_cast<std::underlying_type<type_id>::type>(i->second));
            return;
        }

        if (type->is_reversed()) {
            fail(unimplemented::cause::REVERSED);
        }
        if (type->is_user_type()) {
            r.write_short(uint16_t(type_id::UDT));
            auto udt = static_pointer_cast<const user_type_impl>(type);
            r.write_string(udt->_keyspace);
            r.write_bytes_as_string(udt->_name);
            r.write_short(udt->size());
            for (auto&& i : boost::irange<size_t>(0, udt->size())) {
                r.write_bytes_as_string(udt->field_name(i));
                encode(r, udt->field_type(i));
            }
            return;
        }
        if (type->is_tuple()) {
            r.write_short(uint16_t(type_id::TUPLE));
            auto ttype = static_pointer_cast<const tuple_type_impl>(type);
            r.write_short(ttype->size());
            for (auto&& t : ttype->all_types()) {
                encode(r, t);
            }
            return;
        }
        if (type->is_collection()) {
            auto&& ctype = static_cast<const collection_type_impl*>(type.get());
            if (ctype->get_kind() == abstract_type::kind::map) {
                r.write_short(uint16_t(type_id::MAP));
                auto&& mtype = static_cast<const map_type_impl*>(ctype);
                encode(r, mtype->get_keys_type());
                encode(r, mtype->get_values_type());
            } else if (ctype->get_kind() == abstract_type::kind::set) {
                r.write_short(uint16_t(type_id::SET));
                auto&& stype = static_cast<const set_type_impl*>(ctype);
                encode(r, stype->get_elements_type());
            } else if (ctype->get_kind() == abstract_type::kind::list) {
                r.write_short(uint16_t(type_id::LIST));
                auto&& ltype = static_cast<const list_type_impl*>(ctype);
                encode(r, ltype->get_elements_type());
            } else {
                abort();
            }
            return;
        }
        abort();
    }