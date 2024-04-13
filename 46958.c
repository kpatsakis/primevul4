static DIR *open_cwd(pid_t pid)
{
    char buf[sizeof("/proc/%lu/cwd") + sizeof(long)*3];
    sprintf(buf, "/proc/%lu/cwd", (long)pid);

    DIR *cwd = opendir(buf);
    if (cwd == NULL)
        perror_msg("Can't open process's CWD for CompatCore");

    return cwd;
}
