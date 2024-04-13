weechat_plugin_init (struct t_weechat_plugin *plugin, int argc, char *argv[])
{
    /* make C compiler happy */
    (void) argc;
    (void) argv;

    weechat_plugin = plugin;

    if (!logger_config_init ())
        return WEECHAT_RC_ERROR;

    logger_config_read ();

    /* command /logger */
    weechat_hook_command (
        "logger",
        N_("logger plugin configuration"),
        N_("list"
           " || set <level>"
           " || flush"
           " || disable"),
        N_("   list: show logging status for opened buffers\n"
           "    set: set logging level on current buffer\n"
           "  level: level for messages to be logged (0 = logging disabled, "
           "1 = a few messages (most important) .. 9 = all messages)\n"
           "  flush: write all log files now\n"
           "disable: disable logging on current buffer (set level to 0)\n"
           "\n"
           "Options \"logger.level.*\" and \"logger.mask.*\" can be used to set "
           "level or mask for a buffer, or buffers beginning with name.\n"
           "\n"
           "Log levels used by IRC plugin:\n"
           "  1: user message, notice, private\n"
           "  2: nick change\n"
           "  3: server message\n"
           "  4: join/part/quit\n"
           "  9: all other messages\n"
           "\n"
           "Examples:\n"
           "  set level to 5 for current buffer:\n"
           "    /logger set 5\n"
           "  disable logging for current buffer:\n"
           "    /logger disable\n"
           "  set level to 3 for all IRC buffers:\n"
           "    /set logger.level.irc 3\n"
           "  disable logging for main WeeChat buffer:\n"
           "    /set logger.level.core.weechat 0\n"
           "  use a directory per IRC server and a file per channel inside:\n"
           "    /set logger.mask.irc \"$server/$channel.weechatlog\""),
        "list"
        " || set 1|2|3|4|5|6|7|8|9"
        " || flush"
        " || disable",
        &logger_command_cb, NULL, NULL);

    logger_start_buffer_all (1);

    weechat_hook_signal ("buffer_opened",
                         &logger_buffer_opened_signal_cb, NULL, NULL);
    weechat_hook_signal ("buffer_closing",
                         &logger_buffer_closing_signal_cb, NULL, NULL);
    weechat_hook_signal ("buffer_renamed",
                         &logger_buffer_renamed_signal_cb, NULL, NULL);
    weechat_hook_signal ("logger_backlog",
                         &logger_backlog_signal_cb, NULL, NULL);
    weechat_hook_signal ("logger_start",
                         &logger_start_signal_cb, NULL, NULL);
    weechat_hook_signal ("logger_stop",
                         &logger_stop_signal_cb, NULL, NULL);
    weechat_hook_signal ("day_changed",
                         &logger_day_changed_signal_cb, NULL, NULL);

    weechat_hook_print (NULL, NULL, NULL, 1, &logger_print_cb, NULL, NULL);

    logger_info_init ();

    return WEECHAT_RC_OK;
}
