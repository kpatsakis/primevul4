logger_stop (struct t_logger_buffer *logger_buffer, int write_info_line)
{
    time_t seconds;
    struct tm *date_tmp;
    char buf_time[256];

    if (!logger_buffer)
        return;

    if (logger_buffer->log_enabled && logger_buffer->log_file)
    {
        if (write_info_line && weechat_config_boolean (logger_config_file_info_lines))
        {
            buf_time[0] = '\0';
            seconds = time (NULL);
            date_tmp = localtime (&seconds);
            if (date_tmp)
            {
                strftime (buf_time, sizeof (buf_time) - 1,
                          weechat_config_string (logger_config_file_time_format),
                          date_tmp);
            }
            logger_write_line (logger_buffer,
                               _("%s\t****  End of log  ****"),
                               buf_time);
        }
        fclose (logger_buffer->log_file);
        logger_buffer->log_file = NULL;
    }
    logger_buffer_free (logger_buffer);
}
