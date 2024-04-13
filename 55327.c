logger_set_buffer (struct t_gui_buffer *buffer, const char *value)
{
    char *name;
    struct t_config_option *ptr_option;

    name = logger_build_option_name (buffer);
    if (!name)
        return;

    if (logger_config_set_level (name, value) != WEECHAT_CONFIG_OPTION_SET_ERROR)
    {
        ptr_option = logger_config_get_level (name);
        if (ptr_option)
        {
            weechat_printf (NULL, _("%s: \"%s\" => level %d"),
                            LOGGER_PLUGIN_NAME, name,
                            weechat_config_integer (ptr_option));
        }
    }

    free (name);
}
