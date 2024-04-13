    virtual void visit(const messages::result_message::set_keyspace& m) override {
        _response.write_int(0x0003);
        _response.write_string(m.get_keyspace());
    }