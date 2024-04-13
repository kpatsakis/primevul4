logger_get_level_for_buffer (struct t_gui_buffer *buffer)
{
    const char *no_log;
    char *name, *option_name, *ptr_end;
    struct t_config_option *ptr_option;

    /* no log for buffer if local variable "no_log" is defined for buffer */
    no_log = weechat_buffer_get_string (buffer, "localvar_no_log");
    if (no_log && no_log[0])
        return 0;

    name = logger_build_option_name (buffer);
    if (!name)
        return LOGGER_LEVEL_DEFAULT;

    option_name = strdup (name);
    if (option_name)
    {
        ptr_end = option_name + strlen (option_name);
        while (ptr_end >= option_name)
        {
            ptr_option = logger_config_get_level (option_name);
            if (ptr_option)
            {
                free (option_name);
                free (name);
                return weechat_config_integer (ptr_option);
            }
            ptr_end--;
            while ((ptr_end >= option_name) && (ptr_end[0] != '.'))
            {
                ptr_end--;
            }
            if ((ptr_end >= option_name) && (ptr_end[0] == '.'))
                ptr_end[0] = '\0';
        }
        ptr_option = logger_config_get_level (option_name);

        free (option_name);
        free (name);

        if (ptr_option)
            return weechat_config_integer (ptr_option);
    }
    else
        free (name);

    /* nothing found => return default level */
    return LOGGER_LEVEL_DEFAULT;
}
