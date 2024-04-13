    virtual void visit(const messages::result_message::schema_change& m) override {
        auto change = m.get_change();
        switch (change->type) {
        case event::event_type::SCHEMA_CHANGE: {
            auto sc = static_pointer_cast<event::schema_change>(change);
            _response.write_int(0x0005);
            _response.serialize(*sc, _version);
            break;
        }
        default:
            assert(0);
        }
    }