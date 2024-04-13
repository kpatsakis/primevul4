static int zlib_decompress_update(struct crypto_pcomp *tfm,
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

	ret = zlib_inflate(stream, Z_SYNC_FLUSH);
	switch (ret) {
	case Z_OK:
	case Z_STREAM_END:
		break;

	case Z_BUF_ERROR:
		pr_debug("zlib_inflate could not make progress\n");
		return -EAGAIN;

	default:
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
