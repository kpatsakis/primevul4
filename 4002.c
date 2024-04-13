void cql_server::response::compress_snappy()
{
    using namespace compression_buffers;
    auto view = input_buffer.get_linearized_view(_body);
    const char* input = reinterpret_cast<const char*>(view.data());
    size_t input_len = view.size();

    size_t output_len = snappy_max_compressed_length(input_len);
    _body = output_buffer.make_buffer(output_len, [&] (bytes_mutable_view output_view) {
        char* output = reinterpret_cast<char*>(output_view.data());
        if (snappy_compress(input, input_len, output, &output_len) != SNAPPY_OK) {
            throw std::runtime_error("CQL frame Snappy compression failure");
        }
        return output_len;
    });
    on_compression_buffer_use();
}