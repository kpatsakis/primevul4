mapped_v4_to_regular_v4(char *str)
{
    char *prefix = "::ffff:";
    int prefix_len;

    prefix_len = strlen(prefix);
    if (strncmp(str, prefix, prefix_len) == 0) {
	int str_len = strlen(str);
	memmove(str, str + prefix_len, str_len - prefix_len + 1);
    }
}
