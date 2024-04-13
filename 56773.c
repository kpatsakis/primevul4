static int init_entropy_decoder(APEContext *ctx)
{
    /* Read the CRC */
    if (ctx->fileversion >= 3900) {
        if (ctx->data_end - ctx->ptr < 6)
            return AVERROR_INVALIDDATA;
        ctx->CRC = bytestream_get_be32(&ctx->ptr);
    } else {
        ctx->CRC = get_bits_long(&ctx->gb, 32);
    }

    /* Read the frame flags if they exist */
    ctx->frameflags = 0;
    if ((ctx->fileversion > 3820) && (ctx->CRC & 0x80000000)) {
        ctx->CRC &= ~0x80000000;

        if (ctx->data_end - ctx->ptr < 6)
            return AVERROR_INVALIDDATA;
        ctx->frameflags = bytestream_get_be32(&ctx->ptr);
    }

    /* Initialize the rice structs */
    ctx->riceX.k = 10;
    ctx->riceX.ksum = (1 << ctx->riceX.k) * 16;
    ctx->riceY.k = 10;
    ctx->riceY.ksum = (1 << ctx->riceY.k) * 16;

    if (ctx->fileversion >= 3900) {
        /* The first 8 bits of input are ignored. */
        ctx->ptr++;

        range_start_decoding(ctx);
    }

    return 0;
}
