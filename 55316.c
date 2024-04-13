logger_buffer_opened_signal_cb (const void *pointer, void *data,
                                const char *signal,
                                const char *type_data, void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    logger_start_buffer (signal_data, 1);

    return WEECHAT_RC_OK;
}
