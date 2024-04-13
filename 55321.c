logger_get_file_path ()
{
    char *path, *path2;
    int length;
    time_t seconds;
    struct tm *date_tmp;

    path = NULL;
    path2 = NULL;

    /* replace %h and "~", evaluate path */
    path = weechat_string_eval_path_home (
        weechat_config_string (logger_config_file_path), NULL, NULL, NULL);
    if (!path)
        goto end;

    /* replace date/time specifiers in path */
    length = strlen (path) + 256 + 1;
    path2 = malloc (length);
    if (!path2)
        goto end;
    seconds = time (NULL);
    date_tmp = localtime (&seconds);
    path2[0] = '\0';
    strftime (path2, length - 1, path, date_tmp);

    if (weechat_logger_plugin->debug)
    {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  "%s: file path = \"%s\"",
                                  LOGGER_PLUGIN_NAME, path2);
    }

end:
    if (path)
        free (path);
    return path2;
}
