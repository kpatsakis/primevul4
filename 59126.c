confirm_action (const char *prompt, ...)
{
    if (INTERACTIVE)
    {
	int confirmed = 0;
	char buf[BUFSIZ + 1];
	va_list args;
	va_start (args, prompt);

	VPRINTF(stdout, prompt, args);
	fgets (buf, BUFSIZ, stdin);
	if (buf[0] == 'y' || buf[0] == 'Y') confirmed = 1;

	va_end (args);

	return confirmed;
    }
    return 1;
}
