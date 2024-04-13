irc_ctcp_reply_to_nick (struct t_irc_server *server,
                        const char *command,
                        struct t_irc_channel *channel,
                        const char *nick, const char *ctcp,
                        const char *arguments)
{
    struct t_hashtable *hashtable;
    int number;
    char hash_key[32];
    const char *str_args;
    char *str_args_color;

    hashtable = irc_server_sendf (
        server,
        IRC_SERVER_SEND_OUTQ_PRIO_LOW | IRC_SERVER_SEND_RETURN_HASHTABLE,
        NULL,
        "NOTICE %s :\01%s%s%s\01",
        nick, ctcp,
        (arguments) ? " " : "",
        (arguments) ? arguments : "");

    if (hashtable)
    {
        if (weechat_config_boolean (irc_config_look_display_ctcp_reply))
        {
            number = 1;
            while (1)
            {
                snprintf (hash_key, sizeof (hash_key), "args%d", number);
                str_args = weechat_hashtable_get (hashtable, hash_key);
                if (!str_args)
                    break;
                str_args_color = irc_color_decode (str_args, 1);
                if (!str_args_color)
                    break;
                weechat_printf_date_tags (
                    irc_msgbuffer_get_target_buffer (
                        server, nick, NULL, "ctcp",
                        (channel) ? channel->buffer : NULL),
                    0,
                    irc_protocol_tags (
                        command,
                        "irc_ctcp,irc_ctcp_reply,self_msg,notify_none,"
                        "no_highlight",
                        NULL, NULL),
                    _("%sCTCP reply to %s%s%s: %s%s%s%s%s"),
                    weechat_prefix ("network"),
                    irc_nick_color_for_msg (server, 0, NULL, nick),
                    nick,
                    IRC_COLOR_RESET,
                    IRC_COLOR_CHAT_CHANNEL,
                    ctcp,
                    (str_args_color[0]) ? IRC_COLOR_RESET : "",
                    (str_args_color[0]) ? " " : "",
                    str_args_color);
                free (str_args_color);
                number++;
            }
        }
        weechat_hashtable_free (hashtable);
    }
}
