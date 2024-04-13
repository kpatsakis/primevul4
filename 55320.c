logger_flush ()
{
    struct t_logger_buffer *ptr_logger_buffer;

    for (ptr_logger_buffer = logger_buffers; ptr_logger_buffer;
         ptr_logger_buffer = ptr_logger_buffer->next_buffer)
    {
        if (ptr_logger_buffer->log_file && ptr_logger_buffer->flush_needed)
        {
            if (weechat_logger_plugin->debug >= 2)
            {
                weechat_printf_date_tags (NULL, 0, "no_log",
                                          "%s: flush file %s",
                                          LOGGER_PLUGIN_NAME,
                                          ptr_logger_buffer->log_filename);
            }
            fflush (ptr_logger_buffer->log_file);
            ptr_logger_buffer->flush_needed = 0;
        }
    }
}
