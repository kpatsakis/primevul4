_checksum_init(struct chksumwork *sumwrk, int sum_alg)
{
	sumwrk->alg = sum_alg;
	switch (sum_alg) {
	case CKSUM_NONE:
		break;
	case CKSUM_SHA1:
		archive_sha1_init(&(sumwrk->sha1ctx));
		break;
	case CKSUM_MD5:
		archive_md5_init(&(sumwrk->md5ctx));
		break;
	}
}
