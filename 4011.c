    virtual void visit(const messages::result_message::void_message&) override {
        _response.write_int(0x0001);
    }