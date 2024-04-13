logger_write_line (struct t_logger_buffer *logger_buffer,
                   const char *format, ...)
{
    char *message, buf_time[256], buf_beginning[1024];
    const char *charset;
    time_t seconds;
    struct tm *date_tmp;
    int log_level;

    charset = weechat_info_get ("charset_terminal", "");

    if (!logger_buffer->log_file)
    {
        log_level = logger_get_level_for_buffer (logger_buffer->buffer);
        if (log_level == 0)
        {
            logger_buffer_free (logger_buffer);
            return;
        }
        if (!logger_create_directory ())
        {
            weechat_printf_date_tags (
                NULL, 0, "no_log",
                _("%s%s: unable to create directory for logs "
                  "(\"%s\")"),
                weechat_prefix ("error"), LOGGER_PLUGIN_NAME,
                weechat_config_string (logger_config_file_path));
            logger_buffer_free (logger_buffer);
            return;
        }
        if (!logger_buffer->log_filename)
            logger_set_log_filename (logger_buffer);
        if (!logger_buffer->log_filename)
        {
            logger_buffer_free (logger_buffer);
            return;
        }

        logger_buffer->log_file =
            fopen (logger_buffer->log_filename, "a");
        if (!logger_buffer->log_file)
        {
            weechat_printf_date_tags (
                NULL, 0, "no_log",
                _("%s%s: unable to write log file \"%s\": %s"),
                weechat_prefix ("error"), LOGGER_PLUGIN_NAME,
                logger_buffer->log_filename, strerror (errno));
            logger_buffer_free (logger_buffer);
            return;
        }

        if (weechat_config_boolean (logger_config_file_info_lines)
            && logger_buffer->write_start_info_line)
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
            snprintf (buf_beginning, sizeof (buf_beginning),
                      _("%s\t****  Beginning of log  ****"),
                      buf_time);
            message = (charset) ?
                weechat_iconv_from_internal (charset, buf_beginning) : NULL;
            fprintf (logger_buffer->log_file,
                     "%s\n", (message) ? message : buf_beginning);
            if (message)
                free (message);
            logger_buffer->flush_needed = 1;
        }
        logger_buffer->write_start_info_line = 0;
    }

    weechat_va_format (format);
    if (vbuffer)
    {
        message = (charset) ?
            weechat_iconv_from_internal (charset, vbuffer) : NULL;
        fprintf (logger_buffer->log_file,
                 "%s\n", (message) ? message : vbuffer);
        if (message)
            free (message);
        logger_buffer->flush_needed = 1;
        if (!logger_timer)
        {
            fflush (logger_buffer->log_file);
            logger_buffer->flush_needed = 0;
        }
        free (vbuffer);
    }
}
