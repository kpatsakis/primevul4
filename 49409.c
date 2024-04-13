static void sha1_apply_transform_avx2(u32 *digest, const char *data,
				unsigned int rounds)
{
	/* Select the optimal transform based on data block size */
	if (rounds >= SHA1_AVX2_BLOCK_OPTSIZE)
		sha1_transform_avx2(digest, data, rounds);
	else
		sha1_transform_avx(digest, data, rounds);
}
