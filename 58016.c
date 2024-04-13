irc_ctcp_get_default_reply (const char *ctcp)
{
    int i;

    for (i = 0; irc_ctcp_default_reply[i].name; i++)
    {
        if (weechat_strcasecmp (irc_ctcp_default_reply[i].name, ctcp) == 0)
            return irc_ctcp_default_reply[i].reply;
    }

    /* unknown CTCP */
    return NULL;
}
