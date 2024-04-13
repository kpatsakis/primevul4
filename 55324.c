logger_get_line_tag_info (int tags_count, const char **tags,
                          int *log_level, int *prefix_is_nick)
{
    int i, log_level_set, prefix_is_nick_set;

    if (log_level)
        *log_level = LOGGER_LEVEL_DEFAULT;
    if (prefix_is_nick)
        *prefix_is_nick = 0;

    log_level_set = 0;
    prefix_is_nick_set = 0;

    for (i = 0; i < tags_count; i++)
    {
        if (log_level && !log_level_set)
        {
            if (strcmp (tags[i], "no_log") == 0)
            {
                /* log disabled on line: set level to -1 */
                *log_level = -1;
                log_level_set = 1;
            }
            else if (strncmp (tags[i], "log", 3) == 0)
            {
                /* set log level for line */
                if (isdigit ((unsigned char)tags[i][3]))
                {
                    *log_level = (tags[i][3] - '0');
                    log_level_set = 1;
                }
            }
        }
        if (prefix_is_nick && !prefix_is_nick_set)
        {
            if (strncmp (tags[i], "prefix_nick", 11) == 0)
            {
                *prefix_is_nick = 1;
                prefix_is_nick_set = 1;
            }
        }
    }
}
