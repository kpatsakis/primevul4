mm_answer_moduli(int sock, Buffer *m)
{
	DH *dh;
	int min, want, max;

	min = buffer_get_int(m);
	want = buffer_get_int(m);
	max = buffer_get_int(m);

	debug3("%s: got parameters: %d %d %d",
	    __func__, min, want, max);
	/* We need to check here, too, in case the child got corrupted */
	if (max < min || want < min || max < want)
		fatal("%s: bad parameters: %d %d %d",
		    __func__, min, want, max);

	buffer_clear(m);

	dh = choose_dh(min, want, max);
	if (dh == NULL) {
		buffer_put_char(m, 0);
		return (0);
	} else {
		/* Send first bignum */
		buffer_put_char(m, 1);
		buffer_put_bignum2(m, dh->p);
		buffer_put_bignum2(m, dh->g);

		DH_free(dh);
	}
	mm_request_send(sock, MONITOR_ANS_MODULI, m);
	return (0);
}
