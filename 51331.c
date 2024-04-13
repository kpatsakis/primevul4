static void sig_winch(int sig) {
	unsigned short width,height;
	struct mt_packet data;
	int plen;

	/* terminal height/width has changed, inform server */
	if (get_terminal_size(&width, &height) != -1) {
		init_packet(&data, MT_PTYPE_DATA, srcmac, dstmac, sessionkey, outcounter);
		width = htole16(width);
		height = htole16(height);
		plen = add_control_packet(&data, MT_CPTYPE_TERM_WIDTH, &width, 2);
		plen += add_control_packet(&data, MT_CPTYPE_TERM_HEIGHT, &height, 2);
		outcounter += plen;

		send_udp(&data, 1);
	}

	/* reinstate signal handler */
	signal(SIGWINCH, sig_winch);
}
