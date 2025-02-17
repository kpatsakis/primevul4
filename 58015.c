irc_ctcp_display_request (struct t_irc_server *server,
                          time_t date,
                          const char *command,
                          struct t_irc_channel *channel,
                          const char *nick,
                          const char *address,
                          const char *ctcp,
                          const char *arguments,
                          const char *reply)
{
    /* CTCP blocked and user doesn't want to see message? then just return */
    if (reply && !reply[0]
        && !weechat_config_boolean (irc_config_look_display_ctcp_blocked))
        return;

    weechat_printf_date_tags (
        irc_msgbuffer_get_target_buffer (
            server, nick, NULL, "ctcp",
            (channel) ? channel->buffer : NULL),
        date,
        irc_protocol_tags (command, "irc_ctcp", NULL, address),
        _("%sCTCP requested by %s%s%s: %s%s%s%s%s%s"),
        weechat_prefix ("network"),
        irc_nick_color_for_msg (server, 0, NULL, nick),
        nick,
        IRC_COLOR_RESET,
        IRC_COLOR_CHAT_CHANNEL,
        ctcp,
        IRC_COLOR_RESET,
        (arguments) ? " " : "",
        (arguments) ? arguments : "",
        (reply && !reply[0]) ? _(" (blocked)") : "");
}
