static int zlib_decompress_final(struct crypto_pcomp *tfm,
				 struct comp_request *req)
{
	int ret;
	struct zlib_ctx *dctx = crypto_tfm_ctx(crypto_pcomp_tfm(tfm));
	struct z_stream_s *stream = &dctx->decomp_stream;

	pr_debug("avail_in %u, avail_out %u\n", req->avail_in, req->avail_out);
	stream->next_in = req->next_in;
	stream->avail_in = req->avail_in;
	stream->next_out = req->next_out;
	stream->avail_out = req->avail_out;

	if (dctx->decomp_windowBits < 0) {
		ret = zlib_inflate(stream, Z_SYNC_FLUSH);
		/*
		 * Work around a bug in zlib, which sometimes wants to taste an
		 * extra byte when being used in the (undocumented) raw deflate
		 * mode. (From USAGI).
		 */
		if (ret == Z_OK && !stream->avail_in && stream->avail_out) {
			const void *saved_next_in = stream->next_in;
			u8 zerostuff = 0;

			stream->next_in = &zerostuff;
			stream->avail_in = 1;
			ret = zlib_inflate(stream, Z_FINISH);
			stream->next_in = saved_next_in;
			stream->avail_in = 0;
		}
	} else
		ret = zlib_inflate(stream, Z_FINISH);
	if (ret != Z_STREAM_END) {
		pr_debug("zlib_inflate failed %d\n", ret);
		return -EINVAL;
	}

	ret = req->avail_out - stream->avail_out;
	pr_debug("avail_in %lu, avail_out %lu (consumed %lu, produced %u)\n",
		 stream->avail_in, stream->avail_out,
		 req->avail_in - stream->avail_in, ret);
	req->next_in = stream->next_in;
	req->avail_in = stream->avail_in;
	req->next_out = stream->next_out;
	req->avail_out = stream->avail_out;
	return ret;
}
