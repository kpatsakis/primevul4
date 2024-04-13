irc_ctcp_get_reply (struct t_irc_server *server, const char *ctcp)
{
    struct t_config_option *ptr_option;
    char option_name[512];

    snprintf (option_name, sizeof (option_name), "%s.%s", server->name, ctcp);

    /* search for CTCP in configuration file, for server */
    ptr_option = weechat_config_search_option (irc_config_file,
                                               irc_config_section_ctcp,
                                               option_name);
    if (ptr_option)
        return weechat_config_string (ptr_option);

    /* search for CTCP in configuration file */
    ptr_option = weechat_config_search_option (irc_config_file,
                                               irc_config_section_ctcp,
                                               ctcp);
    if (ptr_option)
        return weechat_config_string (ptr_option);

    /*
     * no CTCP reply found in config, then return default reply, or NULL
     * for unknown CTCP
     */
    return irc_ctcp_get_default_reply (ctcp);
}
