logger_create_directory ()
{
    int rc;
    char *file_path;

    rc = 1;

    file_path = logger_get_file_path ();
    if (file_path)
    {
        if (!weechat_mkdir_parents (file_path, 0700))
            rc = 0;
        free (file_path);
    }
    else
        rc = 0;

    return rc;
}
