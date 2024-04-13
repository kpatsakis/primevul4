irc_ctcp_display_reply_from_nick (struct t_irc_server *server, time_t date,
                                  const char *command, const char *nick,
                                  const char *address, char *arguments)
{
    char *pos_end, *pos_space, *pos_args, *pos_usec;
    struct timeval tv;
    long sec1, usec1, sec2, usec2, difftime;

    while (arguments && arguments[0])
    {
        pos_end = strrchr (arguments + 1, '\01');
        if (pos_end)
            pos_end[0] = '\0';

        pos_space = strchr (arguments + 1, ' ');
        if (pos_space)
        {
            pos_space[0] = '\0';
            pos_args = pos_space + 1;
            while (pos_args[0] == ' ')
            {
                pos_args++;
            }
            if (strcmp (arguments + 1, "PING") == 0)
            {
                pos_usec = strchr (pos_args, ' ');
                if (pos_usec)
                {
                    pos_usec[0] = '\0';

                    gettimeofday (&tv, NULL);
                    sec1 = atol (pos_args);
                    usec1 = atol (pos_usec + 1);
                    sec2 = tv.tv_sec;
                    usec2 = tv.tv_usec;

                    difftime = ((sec2 * 1000000) + usec2) -
                        ((sec1 * 1000000) + usec1);
                    weechat_printf_date_tags (
                        irc_msgbuffer_get_target_buffer (
                            server, nick, NULL, "ctcp", NULL),
                        date,
                        irc_protocol_tags (command, "irc_ctcp", NULL, NULL),
                        /* TRANSLATORS: %.3fs is a float number + "s" ("seconds") */
                        _("%sCTCP reply from %s%s%s: %s%s%s %.3fs"),
                        weechat_prefix ("network"),
                        irc_nick_color_for_msg (server, 0, NULL, nick),
                        nick,
                        IRC_COLOR_RESET,
                        IRC_COLOR_CHAT_CHANNEL,
                        arguments + 1,
                        IRC_COLOR_RESET,
                        (float)difftime / 1000000.0);

                    pos_usec[0] = ' ';
                }
            }
            else
            {
                weechat_printf_date_tags (
                    irc_msgbuffer_get_target_buffer (
                        server, nick, NULL, "ctcp", NULL),
                    date,
                    irc_protocol_tags (command, "irc_ctcp", NULL, address),
                    _("%sCTCP reply from %s%s%s: %s%s%s%s%s"),
                    weechat_prefix ("network"),
                    irc_nick_color_for_msg (server, 0, NULL, nick),
                    nick,
                    IRC_COLOR_RESET,
                    IRC_COLOR_CHAT_CHANNEL,
                    arguments + 1,
                    IRC_COLOR_RESET,
                    " ",
                    pos_args);
            }
            pos_space[0] = ' ';
        }
        else
        {
            weechat_printf_date_tags (
                irc_msgbuffer_get_target_buffer (
                    server, nick, NULL, "ctcp", NULL),
                date,
                irc_protocol_tags (command, NULL, NULL, address),
                _("%sCTCP reply from %s%s%s: %s%s%s%s%s"),
                weechat_prefix ("network"),
                irc_nick_color_for_msg (server, 0, NULL, nick),
                nick,
                IRC_COLOR_RESET,
                IRC_COLOR_CHAT_CHANNEL,
                arguments + 1,
                "",
                "",
                "");
        }

        if (pos_end)
            pos_end[0] = '\01';

        arguments = (pos_end) ? pos_end + 1 : NULL;
    }
}
