_checksum_final(struct chksumwork *sumwrk, const void *val, size_t len)
{
	unsigned char sum[MAX_SUM_SIZE];
	int r = ARCHIVE_OK;

	switch (sumwrk->alg) {
	case CKSUM_NONE:
		break;
	case CKSUM_SHA1:
		archive_sha1_final(&(sumwrk->sha1ctx), sum);
		if (len != SHA1_SIZE ||
		    memcmp(val, sum, SHA1_SIZE) != 0)
			r = ARCHIVE_FAILED;
		break;
	case CKSUM_MD5:
		archive_md5_final(&(sumwrk->md5ctx), sum);
		if (len != MD5_SIZE ||
		    memcmp(val, sum, MD5_SIZE) != 0)
			r = ARCHIVE_FAILED;
		break;
	}
	return (r);
}
