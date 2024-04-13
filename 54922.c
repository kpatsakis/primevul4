get_action(char *buf, int len)
{
    char *action;
    int pos = 0;

    while (pos < len && isspace((unsigned char)buf[pos]))
        pos++;
    if (pos == len) {
        return NULL;
    }
    action = buf + pos;

    while (pos < len && (!isspace((unsigned char)buf[pos]) && buf[pos] != ':'))
        pos++;
    buf[pos] = '\0';

    return action;
}
