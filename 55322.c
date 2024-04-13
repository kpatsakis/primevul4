logger_get_filename (struct t_gui_buffer *buffer)
{
    char *res, *mask_expanded, *file_path;
    const char *mask;
    const char *dir_separator, *weechat_dir;
    int length;

    res = NULL;
    mask_expanded = NULL;
    file_path = NULL;

    dir_separator = weechat_info_get ("dir_separator", "");
    if (!dir_separator)
        return NULL;
    weechat_dir = weechat_info_get ("weechat_dir", "");
    if (!weechat_dir)
        return NULL;

    /* get filename mask for buffer */
    mask = logger_get_mask_for_buffer (buffer);
    if (!mask)
    {
        weechat_printf_date_tags (
            NULL, 0, "no_log",
            _("%s%s: unable to find filename mask for buffer "
              "\"%s\", logging is disabled for this buffer"),
            weechat_prefix ("error"), LOGGER_PLUGIN_NAME,
            weechat_buffer_get_string (buffer, "name"));
        return NULL;
    }

    mask_expanded = logger_get_mask_expanded (buffer, mask);
    if (!mask_expanded)
        goto end;

    file_path = logger_get_file_path ();
    if (!file_path)
        goto end;

    /* build string with path + mask */
    length = strlen (file_path) + strlen (dir_separator) +
        strlen (mask_expanded) + 1;
    res = malloc (length);
    if (res)
    {
        snprintf (res, length, "%s%s%s",
                  file_path,
                  (file_path[strlen (file_path) - 1] == dir_separator[0]) ? "" : dir_separator,
                  mask_expanded);
    }

end:
    if (mask_expanded)
        free (mask_expanded);
    if (file_path)
        free (file_path);

    return res;
}
