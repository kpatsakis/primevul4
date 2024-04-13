static int wp512_final(struct shash_desc *desc, u8 *out)
{
	struct wp512_ctx *wctx = shash_desc_ctx(desc);
	int i;
   	u8 *buffer      = wctx->buffer;
   	u8 *bitLength   = wctx->bitLength;
   	int bufferBits  = wctx->bufferBits;
   	int bufferPos   = wctx->bufferPos;
	__be64 *digest  = (__be64 *)out;

   	buffer[bufferPos] |= 0x80U >> (bufferBits & 7);
   	bufferPos++;
   	if (bufferPos > WP512_BLOCK_SIZE - WP512_LENGTHBYTES) {
   		if (bufferPos < WP512_BLOCK_SIZE) {
	   	memset(&buffer[bufferPos], 0, WP512_BLOCK_SIZE - bufferPos);
   		}
   		wp512_process_buffer(wctx);
   		bufferPos = 0;
   	}
   	if (bufferPos < WP512_BLOCK_SIZE - WP512_LENGTHBYTES) {
   		memset(&buffer[bufferPos], 0,
			  (WP512_BLOCK_SIZE - WP512_LENGTHBYTES) - bufferPos);
   	}
   	bufferPos = WP512_BLOCK_SIZE - WP512_LENGTHBYTES;
   	memcpy(&buffer[WP512_BLOCK_SIZE - WP512_LENGTHBYTES],
		   bitLength, WP512_LENGTHBYTES);
   	wp512_process_buffer(wctx);
	for (i = 0; i < WP512_DIGEST_SIZE/8; i++)
		digest[i] = cpu_to_be64(wctx->hash[i]);
   	wctx->bufferBits   = bufferBits;
   	wctx->bufferPos    = bufferPos;

	return 0;
}
