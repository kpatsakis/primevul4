static void cryp_polling_mode(struct cryp_ctx *ctx,
			      struct cryp_device_data *device_data)
{
	int len = ctx->blocksize / BYTES_PER_WORD;
	int remaining_length = ctx->datalen;
	u32 *indata = (u32 *)ctx->indata;
	u32 *outdata = (u32 *)ctx->outdata;

	while (remaining_length > 0) {
		writesl(&device_data->base->din, indata, len);
		indata += len;
		remaining_length -= (len * BYTES_PER_WORD);
		cryp_wait_until_done(device_data);

		readsl(&device_data->base->dout, outdata, len);
		outdata += len;
		cryp_wait_until_done(device_data);
	}
}
