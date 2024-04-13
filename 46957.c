static int create_user_core(int user_core_fd, pid_t pid, off_t ulimit_c)
{
    int err = 1;
    if (user_core_fd >= 0)
    {
        off_t core_size = copyfd_size(STDIN_FILENO, user_core_fd, ulimit_c, COPYFD_SPARSE);
        if (close_user_core(user_core_fd, core_size) != 0)
            goto finito;

        log_notice("Saved core dump of pid %lu to '%s' at '%s' (%llu bytes)", (long)pid, core_basename, user_pwd, (long long)core_size);
    }
    err = 0;

finito:
    if (proc_cwd != NULL)
    {
        closedir(proc_cwd);
        proc_cwd = NULL;
    }

    return err;
}
