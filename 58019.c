irc_ctcp_recv_dcc (struct t_irc_server *server, const char *nick,
                   const char *arguments, char *message)
{
    char *dcc_args, *pos, *pos_file, *pos_addr, *pos_port, *pos_size;
    char *pos_start_resume, *filename;
    struct t_infolist *infolist;
    struct t_infolist_item *item;
    char charset_modifier[256];

    if (!arguments || !arguments[0])
        return;

    if (strncmp (arguments, "SEND ", 5) == 0)
    {
        arguments += 5;
        while (arguments[0] == ' ')
        {
            arguments++;
        }
        dcc_args = strdup (arguments);

        if (!dcc_args)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: not enough memory for \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            return;
        }

        /* DCC filename */
        pos_file = dcc_args;
        while (pos_file[0] == ' ')
        {
            pos_file++;
        }

        /* look for file size */
        pos_size = strrchr (pos_file, ' ');
        if (!pos_size)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }

        pos = pos_size;
        pos_size++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* look for DCC port */
        pos_port = strrchr (pos_file, ' ');
        if (!pos_port)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }

        pos = pos_port;
        pos_port++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* look for DCC IP address */
        pos_addr = strrchr (pos_file, ' ');
        if (!pos_addr)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }

        pos = pos_addr;
        pos_addr++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* remove double quotes around filename */
        filename = irc_ctcp_dcc_filename_without_quotes (pos_file);

        /* add DCC file via xfer plugin */
        infolist = weechat_infolist_new ();
        if (infolist)
        {
            item = weechat_infolist_new_item (infolist);
            if (item)
            {
                weechat_infolist_new_var_string (item, "plugin_name", weechat_plugin->name);
                weechat_infolist_new_var_string (item, "plugin_id", server->name);
                weechat_infolist_new_var_string (item, "type_string", "file_recv");
                weechat_infolist_new_var_string (item, "protocol_string", "dcc");
                weechat_infolist_new_var_string (item, "remote_nick", nick);
                weechat_infolist_new_var_string (item, "local_nick", server->nick);
                weechat_infolist_new_var_string (item, "filename",
                                                 (filename) ? filename : pos_file);
                weechat_infolist_new_var_string (item, "size", pos_size);
                weechat_infolist_new_var_string (item, "proxy",
                                                 IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_PROXY));
                weechat_infolist_new_var_string (item, "remote_address", pos_addr);
                weechat_infolist_new_var_integer (item, "port", atoi (pos_port));
                (void) weechat_hook_signal_send ("xfer_add",
                                                 WEECHAT_HOOK_SIGNAL_POINTER,
                                                 infolist);
            }
            weechat_infolist_free (infolist);
        }

        (void) weechat_hook_signal_send ("irc_dcc",
                                         WEECHAT_HOOK_SIGNAL_STRING,
                                         message);

        if (filename)
            free (filename);

        free (dcc_args);
    }
    else if (strncmp (arguments, "RESUME ", 7) == 0)
    {
        arguments += 7;
        while (arguments[0] == ' ')
        {
            arguments++;
        }
        dcc_args = strdup (arguments);

        if (!dcc_args)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: not enough memory for \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            return;
        }

        /* DCC filename */
        pos_file = dcc_args;
        while (pos_file[0] == ' ')
        {
            pos_file++;
        }

        /* look for resume start position */
        pos_start_resume = strrchr (pos_file, ' ');
        if (!pos_start_resume)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }
        pos = pos_start_resume;
        pos_start_resume++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* look for DCC port */
        pos_port = strrchr (pos_file, ' ');
        if (!pos_port)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }
        pos = pos_port;
        pos_port++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* remove double quotes around filename */
        filename = irc_ctcp_dcc_filename_without_quotes (pos_file);

        /* accept resume via xfer plugin */
        infolist = weechat_infolist_new ();
        if (infolist)
        {
            item = weechat_infolist_new_item (infolist);
            if (item)
            {
                weechat_infolist_new_var_string (item, "plugin_name", weechat_plugin->name);
                weechat_infolist_new_var_string (item, "plugin_id", server->name);
                weechat_infolist_new_var_string (item, "type_string", "file_recv");
                weechat_infolist_new_var_string (item, "filename",
                                                 (filename) ? filename : pos_file);
                weechat_infolist_new_var_integer (item, "port", atoi (pos_port));
                weechat_infolist_new_var_string (item, "start_resume", pos_start_resume);
                (void) weechat_hook_signal_send ("xfer_accept_resume",
                                                 WEECHAT_HOOK_SIGNAL_POINTER,
                                                 infolist);
            }
            weechat_infolist_free (infolist);
        }

        (void) weechat_hook_signal_send ("irc_dcc",
                                         WEECHAT_HOOK_SIGNAL_STRING,
                                         message);

        if (filename)
            free (filename);

        free (dcc_args);
    }
    else if (strncmp (arguments, "ACCEPT ", 7) == 0)
    {
        arguments += 7;
        while (arguments[0] == ' ')
        {
            arguments++;
        }
        dcc_args = strdup (arguments);

        if (!dcc_args)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: not enough memory for \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            return;
        }

        /* DCC filename */
        pos_file = dcc_args;
        while (pos_file[0] == ' ')
        {
            pos_file++;
        }

        /* look for resume start position */
        pos_start_resume = strrchr (pos_file, ' ');
        if (!pos_start_resume)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }
        pos = pos_start_resume;
        pos_start_resume++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* look for DCC port */
        pos_port = strrchr (pos_file, ' ');
        if (!pos_port)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }
        pos = pos_port;
        pos_port++;
        while (pos[0] == ' ')
        {
            pos--;
        }
        pos[1] = '\0';

        /* remove double quotes around filename */
        filename = irc_ctcp_dcc_filename_without_quotes (pos_file);

        /* resume file via xfer plugin */
        infolist = weechat_infolist_new ();
        if (infolist)
        {
            item = weechat_infolist_new_item (infolist);
            if (item)
            {
                weechat_infolist_new_var_string (item, "plugin_name", weechat_plugin->name);
                weechat_infolist_new_var_string (item, "plugin_id", server->name);
                weechat_infolist_new_var_string (item, "type_string", "file_recv");
                weechat_infolist_new_var_string (item, "filename",
                                                 (filename) ? filename : pos_file);
                weechat_infolist_new_var_integer (item, "port", atoi (pos_port));
                weechat_infolist_new_var_string (item, "start_resume", pos_start_resume);
                (void) weechat_hook_signal_send ("xfer_start_resume",
                                                 WEECHAT_HOOK_SIGNAL_POINTER,
                                                 infolist);
            }
            weechat_infolist_free (infolist);
        }

        (void) weechat_hook_signal_send ("irc_dcc",
                                         WEECHAT_HOOK_SIGNAL_STRING,
                                         message);

        if (filename)
            free (filename);

        free (dcc_args);
    }
    else if (strncmp (arguments, "CHAT ", 5) == 0)
    {
        arguments += 5;
        while (arguments[0] == ' ')
        {
            arguments++;
        }
        dcc_args = strdup (arguments);

        if (!dcc_args)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: not enough memory for \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            return;
        }

        /* CHAT type */
        pos_file = dcc_args;
        while (pos_file[0] == ' ')
        {
            pos_file++;
        }

        /* DCC IP address */
        pos_addr = strchr (pos_file, ' ');
        if (!pos_addr)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }
        pos_addr[0] = '\0';
        pos_addr++;
        while (pos_addr[0] == ' ')
        {
            pos_addr++;
        }

        /* look for DCC port */
        pos_port = strchr (pos_addr, ' ');
        if (!pos_port)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: cannot parse \"%s\" command"),
                weechat_prefix ("error"), IRC_PLUGIN_NAME, "privmsg");
            free (dcc_args);
            return;
        }
        pos_port[0] = '\0';
        pos_port++;
        while (pos_port[0] == ' ')
        {
            pos_port++;
        }

        if (weechat_strcasecmp (pos_file, "chat") != 0)
        {
            weechat_printf (
                server->buffer,
                _("%s%s: unknown DCC CHAT type received from %s%s%s: \"%s\""),
                weechat_prefix ("error"),
                IRC_PLUGIN_NAME,
                irc_nick_color_for_msg (server, 0, NULL, nick),
                nick,
                IRC_COLOR_RESET,
                pos_file);
            free (dcc_args);
            return;
        }

        /* add DCC chat via xfer plugin */
        infolist = weechat_infolist_new ();
        if (infolist)
        {
            item = weechat_infolist_new_item (infolist);
            if (item)
            {
                weechat_infolist_new_var_string (item, "plugin_name", weechat_plugin->name);
                weechat_infolist_new_var_string (item, "plugin_id", server->name);
                weechat_infolist_new_var_string (item, "type_string", "chat_recv");
                weechat_infolist_new_var_string (item, "remote_nick", nick);
                weechat_infolist_new_var_string (item, "local_nick", server->nick);
                snprintf (charset_modifier, sizeof (charset_modifier),
                          "irc.%s.%s", server->name, nick);
                weechat_infolist_new_var_string (item, "charset_modifier", charset_modifier);
                weechat_infolist_new_var_string (item, "proxy",
                                                 IRC_SERVER_OPTION_STRING(server, IRC_SERVER_OPTION_PROXY));
                weechat_infolist_new_var_string (item, "remote_address", pos_addr);
                weechat_infolist_new_var_integer (item, "port", atoi (pos_port));
                (void) weechat_hook_signal_send ("xfer_add",
                                                 WEECHAT_HOOK_SIGNAL_POINTER,
                                                 infolist);
            }
            weechat_infolist_free (infolist);
        }

        (void) weechat_hook_signal_send ("irc_dcc",
                                         WEECHAT_HOOK_SIGNAL_STRING,
                                         message);

        free (dcc_args);
    }
}
