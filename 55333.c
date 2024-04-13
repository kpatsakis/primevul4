logger_stop_all (int write_info_line)
{
    while (logger_buffers)
    {
        logger_stop (logger_buffers, write_info_line);
    }
}
