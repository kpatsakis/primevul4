logger_get_mask_for_buffer (struct t_gui_buffer *buffer)
{
    char *name, *option_name, *ptr_end;
    struct t_config_option *ptr_option;

    name = logger_build_option_name (buffer);
    if (!name)
        return NULL;

    option_name = strdup (name);
    if (option_name)
    {
        ptr_end = option_name + strlen (option_name);
        while (ptr_end >= option_name)
        {
            ptr_option = logger_config_get_mask (option_name);
            if (ptr_option)
            {
                free (option_name);
                free (name);
                return weechat_config_string (ptr_option);
            }
            ptr_end--;
            while ((ptr_end >= option_name) && (ptr_end[0] != '.'))
            {
                ptr_end--;
            }
            if ((ptr_end >= option_name) && (ptr_end[0] == '.'))
                ptr_end[0] = '\0';
        }
        ptr_option = logger_config_get_mask (option_name);

        free (option_name);
        free (name);

        if (ptr_option)
            return weechat_config_string (ptr_option);
    }
    else
        free (name);

    /* nothing found => return default mask (if set) */
    if (weechat_config_string (logger_config_file_mask)
        && weechat_config_string (logger_config_file_mask)[0])
        return weechat_config_string (logger_config_file_mask);

    /* no default mask set */
    return NULL;
}
