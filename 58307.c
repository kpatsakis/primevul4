int feh_wm_get_num_desks(void)
{
	char *buf, *ptr;
	int desks;

	if (!feh_wm_get_wm_is_e())
		return(-1);

	buf = enl_send_and_wait("num_desks ?");
	if (buf == IPC_FAKE)	/* Fake E17 IPC */
		return(-1);
	D(("Got from E IPC: %s\n", buf));
	ptr = buf;
	while (ptr && !isdigit(*ptr))
		ptr++;
	desks = atoi(ptr);

	return(desks);
}
