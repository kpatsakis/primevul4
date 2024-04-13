static int close_user_core(int user_core_fd, off_t core_size)
{
    if (user_core_fd >= 0 && (fsync(user_core_fd) != 0 || close(user_core_fd) != 0 || core_size < 0))
    {
        perror_msg("Error writing '%s' at '%s'", core_basename, user_pwd);
        return -1;
    }
    return 0;
}
