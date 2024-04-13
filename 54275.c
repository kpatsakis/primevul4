ASN1_TIME_snprintf (char *buf, int buf_len, ASN1_TIME * tm)
{
	char *expires = NULL;
	BIO *inMem = BIO_new (BIO_s_mem ());

	ASN1_TIME_print (inMem, tm);
	BIO_get_mem_data (inMem, &expires);
	buf[0] = 0;
	if (expires != NULL)
	{
		/* expires is not \0 terminated */
		safe_strcpy (buf, expires, MIN(24, buf_len));
	}
	BIO_free (inMem);
}
