irc_ctcp_recv (struct t_irc_server *server, time_t date, const char *command,
               struct t_irc_channel *channel, const char *address,
               const char *nick, const char *remote_nick, char *arguments,
               char *message)
{
    char *pos_end, *pos_space, *pos_args;
    const char *reply;
    char *decoded_reply;
    struct t_irc_channel *ptr_channel;
    struct t_irc_nick *ptr_nick;
    int nick_is_me;

    while (arguments && arguments[0])
    {
        pos_end = strrchr (arguments + 1, '\01');
        if (pos_end)
            pos_end[0] = '\0';

        pos_args = NULL;
        pos_space = strchr (arguments + 1, ' ');
        if (pos_space)
        {
            pos_space[0] = '\0';
            pos_args = pos_space + 1;
            while (pos_args[0] == ' ')
            {
                pos_args++;
            }
        }

        /* CTCP ACTION */
        if (strcmp (arguments + 1, "ACTION") == 0)
        {
            nick_is_me = (irc_server_strcasecmp (server, server->nick, nick) == 0);
            if (channel)
            {
                ptr_nick = irc_nick_search (server, channel, nick);
                irc_channel_nick_speaking_add (channel,
                                               nick,
                                               (pos_args) ?
                                               weechat_string_has_highlight (pos_args,
                                                                             server->nick) : 0);
                irc_channel_nick_speaking_time_remove_old (channel);
                irc_channel_nick_speaking_time_add (server, channel, nick,
                                                    time (NULL));
                weechat_printf_date_tags (
                    channel->buffer,
                    date,
                    irc_protocol_tags (
                        command,
                        (nick_is_me) ?
                        "irc_action,self_msg,notify_none,no_highlight" :
                        "irc_action,notify_message",
                        nick, address),
                    "%s%s%s%s%s%s%s",
                    weechat_prefix ("action"),
                    irc_nick_mode_for_display (server, ptr_nick, 0),
                    (ptr_nick) ? ptr_nick->color : ((nick) ? irc_nick_find_color (nick) : IRC_COLOR_CHAT_NICK),
                    nick,
                    (pos_args) ? IRC_COLOR_RESET : "",
                    (pos_args) ? " " : "",
                    (pos_args) ? pos_args : "");
            }
            else
            {
                ptr_channel = irc_channel_search (server, remote_nick);
                if (!ptr_channel)
                {
                    ptr_channel = irc_channel_new (server,
                                                   IRC_CHANNEL_TYPE_PRIVATE,
                                                   remote_nick, 0, 0);
                    if (!ptr_channel)
                    {
                        weechat_printf (
                            server->buffer,
                            _("%s%s: cannot create new private buffer \"%s\""),
                            weechat_prefix ("error"), IRC_PLUGIN_NAME,
                            remote_nick);
                    }
                }
                if (ptr_channel)
                {
                    if (!ptr_channel->topic)
                        irc_channel_set_topic (ptr_channel, address);

                    weechat_printf_date_tags (
                        ptr_channel->buffer,
                        date,
                        irc_protocol_tags (
                            command,
                            (nick_is_me) ?
                            "irc_action,self_msg,notify_none,no_highlight" :
                            "irc_action,notify_private",
                            nick, address),
                        "%s%s%s%s%s%s",
                        weechat_prefix ("action"),
                        (nick_is_me) ?
                        IRC_COLOR_CHAT_NICK_SELF : irc_nick_color_for_pv (ptr_channel, nick),
                        nick,
                        (pos_args) ? IRC_COLOR_RESET : "",
                        (pos_args) ? " " : "",
                        (pos_args) ? pos_args : "");
                    (void) weechat_hook_signal_send ("irc_pv",
                                                     WEECHAT_HOOK_SIGNAL_STRING,
                                                     message);
                }
            }
        }
        /* CTCP PING */
        else if (strcmp (arguments + 1, "PING") == 0)
        {
            reply = irc_ctcp_get_reply (server, arguments + 1);
            irc_ctcp_display_request (server, date, command, channel, nick,
                                      address, arguments + 1, pos_args, reply);
            if (!reply || reply[0])
            {
                irc_ctcp_reply_to_nick (server, command, channel, nick,
                                        arguments + 1, pos_args);
            }
        }
        /* CTCP DCC */
        else if (strcmp (arguments + 1, "DCC") == 0)
        {
            irc_ctcp_recv_dcc (server, nick, pos_args, message);
        }
        /* other CTCP */
        else
        {
            reply = irc_ctcp_get_reply (server, arguments + 1);
            if (reply)
            {
                irc_ctcp_display_request (server, date, command, channel, nick,
                                          address, arguments + 1, pos_args,
                                          reply);

                if (reply[0])
                {
                    decoded_reply = irc_ctcp_replace_variables (server, reply);
                    if (decoded_reply)
                    {
                        irc_ctcp_reply_to_nick (server, command, channel, nick,
                                                arguments + 1, decoded_reply);
                        free (decoded_reply);
                    }
                }
            }
            else
            {
                if (weechat_config_boolean (irc_config_look_display_ctcp_unknown))
                {
                    weechat_printf_date_tags (
                        irc_msgbuffer_get_target_buffer (
                            server, nick, NULL, "ctcp",
                            (channel) ? channel->buffer : NULL),
                        date,
                        irc_protocol_tags (command, "irc_ctcp", NULL, address),
                        _("%sUnknown CTCP requested by %s%s%s: %s%s%s%s%s"),
                        weechat_prefix ("network"),
                        irc_nick_color_for_msg (server, 0, NULL, nick),
                        nick,
                        IRC_COLOR_RESET,
                        IRC_COLOR_CHAT_CHANNEL,
                        arguments + 1,
                        (pos_args) ? IRC_COLOR_RESET : "",
                        (pos_args) ? " " : "",
                        (pos_args) ? pos_args : "");
                }
            }
        }

        (void) weechat_hook_signal_send ("irc_ctcp",
                                         WEECHAT_HOOK_SIGNAL_STRING,
                                         message);

        if (pos_space)
            pos_space[0] = ' ';

        if (pos_end)
            pos_end[0] = '\01';

        arguments = (pos_end) ? pos_end + 1 : NULL;
    }
}
