crm_buffer_add_char(char **buffer, int *offset, int *max, char c)
{
    buffer_print(*buffer, *max, *offset, "%c", c);
}
