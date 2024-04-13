    virtual void visit(const messages::result_message::rows& m) override {
        _response.write_int(0x0002);
        auto& rs = m.rs();
        _response.write(rs.get_metadata(), _skip_metadata);
        auto row_count_plhldr = _response.write_int_placeholder();

        class visitor {
            cql_server::response& _response;
            int64_t _row_count = 0;
        public:
            visitor(cql_server::response& r) : _response(r) { }

            void start_row() {
                _row_count++;
            }
            void accept_value(std::optional<query::result_bytes_view> cell) {
                _response.write_value(cell);
            }
            void end_row() { }

            int64_t row_count() const { return _row_count; }
        };

        auto v = visitor(_response);
        rs.visit(v);
        row_count_plhldr.write(v.row_count()); // even though the placeholder is for int32_t we won't overflow because of memory limits
    }