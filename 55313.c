logger_backlog (struct t_gui_buffer *buffer, const char *filename, int lines)
{
    const char *charset;
    struct t_logger_line *last_lines, *ptr_lines;
    char *pos_message, *pos_tab, *error, *message;
    time_t datetime, time_now;
    struct tm tm_line;
    int num_lines;

    charset = weechat_info_get ("charset_terminal", "");

    weechat_buffer_set (buffer, "print_hooks_enabled", "0");

    num_lines = 0;
    last_lines = logger_tail_file (filename, lines);
    ptr_lines = last_lines;
    while (ptr_lines)
    {
        datetime = 0;
        pos_message = strchr (ptr_lines->data, '\t');
        if (pos_message)
        {
            /* initialize structure, because strptime does not do it */
            memset (&tm_line, 0, sizeof (struct tm));
            /*
             * we get current time to initialize daylight saving time in
             * structure tm_line, otherwise printed time will be shifted
             * and will not use DST used on machine
             */
            time_now = time (NULL);
            localtime_r (&time_now, &tm_line);
            pos_message[0] = '\0';
            error = strptime (ptr_lines->data,
                              weechat_config_string (logger_config_file_time_format),
                              &tm_line);
            if (error && !error[0] && (tm_line.tm_year > 0))
                datetime = mktime (&tm_line);
            pos_message[0] = '\t';
        }
        pos_message = (pos_message && (datetime != 0)) ?
            pos_message + 1 : ptr_lines->data;
        message = (charset) ?
            weechat_iconv_to_internal (charset, pos_message) : strdup (pos_message);
        if (message)
        {
            pos_tab = strchr (message, '\t');
            if (pos_tab)
                pos_tab[0] = '\0';
            weechat_printf_date_tags (buffer, datetime,
                                      "no_highlight,notify_none,logger_backlog",
                                      "%s%s%s%s%s",
                                      weechat_color (weechat_config_string (logger_config_color_backlog_line)),
                                      message,
                                      (pos_tab) ? "\t" : "",
                                      (pos_tab) ? weechat_color (weechat_config_string (logger_config_color_backlog_line)) : "",
                                      (pos_tab) ? pos_tab + 1 : "");
            if (pos_tab)
                pos_tab[0] = '\t';
            free (message);
        }
        num_lines++;
        ptr_lines = ptr_lines->next_line;
    }
    if (last_lines)
        logger_tail_free (last_lines);
    if (num_lines > 0)
    {
        weechat_printf_date_tags (buffer, datetime,
                                  "no_highlight,notify_none,logger_backlog_end",
                                  _("%s===\t%s========== End of backlog (%d lines) =========="),
                                  weechat_color (weechat_config_string (logger_config_color_backlog_end)),
                                  weechat_color (weechat_config_string (logger_config_color_backlog_end)),
                                  num_lines);
        weechat_buffer_set (buffer, "unread", "");
    }
    weechat_buffer_set (buffer, "print_hooks_enabled", "1");
}
