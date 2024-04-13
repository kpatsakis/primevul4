void on_compression_buffer_use() {
    if (++buffer_use_count == clear_buffers_trigger) {
        input_buffer.clear();
        output_buffer.clear();
        buffer_use_count = 0;
    }
}