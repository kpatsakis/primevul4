static int need_fallback(unsigned int key_len)
{
	switch (key_len) {
	case 16:
		if (!(keylen_flag & AES_KEYLEN_128))
			return 1;
		break;
	case 24:
		if (!(keylen_flag & AES_KEYLEN_192))
			return 1;
		break;
	case 32:
		if (!(keylen_flag & AES_KEYLEN_256))
			return 1;
		break;
	default:
		return -1;
		break;
	}
	return 0;
}
