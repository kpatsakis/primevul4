read_passphrase_hash (const char *passphrase_file,
		      const md_kt_t *digest,
		      uint8_t *output,
		      int len)
{
  unsigned int outlen = 0;
  md_ctx_t md;

  ASSERT (len >= md_kt_size(digest));
  memset (output, 0, len);

  md_ctx_init(&md, digest);

  /* read passphrase file */
  {
    const int min_passphrase_size = 8;
    uint8_t buf[64];
    int total_size = 0;
    int fd = platform_open (passphrase_file, O_RDONLY, 0);

    if (fd == -1)
      msg (M_ERR, "Cannot open passphrase file: '%s'", passphrase_file);

    for (;;)
      {
	int size = read (fd, buf, sizeof (buf));
	if (size == 0)
	  break;
	if (size == -1)
	  msg (M_ERR, "Read error on passphrase file: '%s'",
	       passphrase_file);
	md_ctx_update(&md, buf, size);
	total_size += size;
      }
    close (fd);

    warn_if_group_others_accessible (passphrase_file);

    if (total_size < min_passphrase_size)
      msg (M_FATAL,
	   "Passphrase file '%s' is too small (must have at least %d characters)",
	   passphrase_file, min_passphrase_size);
  }
  md_ctx_final(&md, output);
  md_ctx_cleanup(&md);
  return md_kt_size(digest);
}
