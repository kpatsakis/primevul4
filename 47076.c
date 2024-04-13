int lzxd_set_reference_data(struct lzxd_stream *lzx,
			    struct mspack_system *system,
			    struct mspack_file *input,
			    unsigned int length)
{
    if (!lzx) return MSPACK_ERR_ARGS;

    if (!lzx->is_delta) {
        D(("only LZX DELTA streams support reference data"))
        return MSPACK_ERR_ARGS;
    }
    if (lzx->offset) {
	D(("too late to set reference data after decoding starts"))
	return MSPACK_ERR_ARGS;
    }
    if (length > lzx->window_size) {
	D(("reference length (%u) is longer than the window", length))
	return MSPACK_ERR_ARGS;
    }
    if (length > 0 && (!system || !input)) {
        D(("length > 0 but no system or input"))
        return MSPACK_ERR_ARGS;
    }

    lzx->ref_data_size = length;
    if (length > 0) {
        /* copy reference data */
        unsigned char *pos = &lzx->window[lzx->window_size - length];
	int bytes = system->read(input, pos, length);
        /* length can't be more than 2^25, so no signedness problem */
	if (bytes < (int)length) return MSPACK_ERR_READ;
    }
    lzx->ref_data_size = length;
    return MSPACK_ERR_OK;
}
