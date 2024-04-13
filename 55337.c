weechat_plugin_end (struct t_weechat_plugin *plugin)
{
    /* make C compiler happy */
    (void) plugin;

    if (logger_timer)
    {
        weechat_unhook (logger_timer);
        logger_timer = NULL;
    }

    logger_config_write ();

    logger_stop_all (1);

    logger_config_free ();

    return WEECHAT_RC_OK;
}
