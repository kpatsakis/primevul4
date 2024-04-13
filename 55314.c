logger_backlog_signal_cb (const void *pointer, void *data,
                          const char *signal,
                          const char *type_data, void *signal_data)
{
    struct t_logger_buffer *ptr_logger_buffer;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    if (weechat_config_integer (logger_config_look_backlog) >= 0)
    {
        ptr_logger_buffer = logger_buffer_search_buffer (signal_data);
        if (ptr_logger_buffer && ptr_logger_buffer->log_enabled)
        {
            if (!ptr_logger_buffer->log_filename)
                logger_set_log_filename (ptr_logger_buffer);

            if (ptr_logger_buffer->log_filename)
            {
                ptr_logger_buffer->log_enabled = 0;

                logger_backlog (signal_data,
                                ptr_logger_buffer->log_filename,
                                weechat_config_integer (logger_config_look_backlog));

                ptr_logger_buffer->log_enabled = 1;
            }
        }
    }

    return WEECHAT_RC_OK;
}
