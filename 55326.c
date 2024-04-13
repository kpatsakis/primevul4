logger_list ()
{
    struct t_infolist *ptr_infolist;
    struct t_logger_buffer *ptr_logger_buffer;
    struct t_gui_buffer *ptr_buffer;
    char status[128];

    weechat_printf (NULL, "");
    weechat_printf (NULL, _("Logging on buffers:"));

    ptr_infolist = weechat_infolist_get ("buffer", NULL, NULL);
    if (ptr_infolist)
    {
        while (weechat_infolist_next (ptr_infolist))
        {
            ptr_buffer = weechat_infolist_pointer (ptr_infolist, "pointer");
            if (ptr_buffer)
            {
                ptr_logger_buffer = logger_buffer_search_buffer (ptr_buffer);
                if (ptr_logger_buffer)
                {
                    snprintf (status, sizeof (status),
                              _("logging (level: %d)"),
                              ptr_logger_buffer->log_level);
                }
                else
                {
                    snprintf (status, sizeof (status), "%s", _("not logging"));
                }
                weechat_printf (NULL,
                                "  %s[%s%d%s]%s (%s) %s%s%s: %s%s%s%s",
                                weechat_color("chat_delimiters"),
                                weechat_color("chat"),
                                weechat_infolist_integer (ptr_infolist, "number"),
                                weechat_color("chat_delimiters"),
                                weechat_color("chat"),
                                weechat_infolist_string (ptr_infolist, "plugin_name"),
                                weechat_color("chat_buffer"),
                                weechat_infolist_string (ptr_infolist, "name"),
                                weechat_color("chat"),
                                status,
                                (ptr_logger_buffer) ? " (" : "",
                                (ptr_logger_buffer) ?
                                ((ptr_logger_buffer->log_filename) ?
                                 ptr_logger_buffer->log_filename : _("log not started")) : "",
                                (ptr_logger_buffer) ? ")" : "");
            }
        }
        weechat_infolist_free (ptr_infolist);
    }
}
