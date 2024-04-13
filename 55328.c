logger_set_log_filename (struct t_logger_buffer *logger_buffer)
{
    char *log_filename, *pos_last_sep;
    const char *dir_separator;
    struct t_logger_buffer *ptr_logger_buffer;

    /* get log filename for buffer */
    log_filename = logger_get_filename (logger_buffer->buffer);
    if (!log_filename)
    {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  _("%s%s: not enough memory"),
                                  weechat_prefix ("error"),
                                  LOGGER_PLUGIN_NAME);
        return;
    }

    /* log file is already used by another buffer? */
    ptr_logger_buffer = logger_buffer_search_log_filename (log_filename);
    if (ptr_logger_buffer)
    {
        weechat_printf_date_tags (
            NULL, 0, "no_log",
            _("%s%s: unable to start logging for buffer "
              "\"%s\": filename \"%s\" is already used by "
              "another buffer (check your log settings)"),
            weechat_prefix ("error"),
            LOGGER_PLUGIN_NAME,
            weechat_buffer_get_string (logger_buffer->buffer, "name"),
            log_filename);
        free (log_filename);
        return;
    }

    /* create directory for path in "log_filename" */
    dir_separator = weechat_info_get ("dir_separator", "");
    if (dir_separator)
    {
        pos_last_sep = strrchr (log_filename, dir_separator[0]);
        if (pos_last_sep)
        {
            pos_last_sep[0] = '\0';
            weechat_mkdir_parents (log_filename, 0700);
            pos_last_sep[0] = dir_separator[0];
        }
    }

    /* set log filename */
    logger_buffer->log_filename = log_filename;
}
