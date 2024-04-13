static inline cql_server::result_with_foreign_response_ptr convert_error_message_to_coordinator_result(messages::result_message* msg) {
    return std::move(*dynamic_cast<messages::result_message::exception*>(msg)).get_exception();
}