Bcj2_Decode(struct _7zip *zip, uint8_t *outBuf, size_t outSize)
{
	size_t inPos = 0, outPos = 0;
	const uint8_t *buf0, *buf1, *buf2, *buf3;
	size_t size0, size1, size2, size3;
	const uint8_t *buffer, *bufferLim;
	unsigned int i, j;

	size0 = zip->tmp_stream_bytes_remaining;
	buf0 = zip->tmp_stream_buff + zip->tmp_stream_bytes_avail - size0;
	size1 = zip->sub_stream_bytes_remaining[0];
	buf1 = zip->sub_stream_buff[0] + zip->sub_stream_size[0] - size1;
	size2 = zip->sub_stream_bytes_remaining[1];
	buf2 = zip->sub_stream_buff[1] + zip->sub_stream_size[1] - size2;
	size3 = zip->sub_stream_bytes_remaining[2];
	buf3 = zip->sub_stream_buff[2] + zip->sub_stream_size[2] - size3;

	buffer = buf3;
	bufferLim = buffer + size3;

	if (zip->bcj_state == 0) {
		/*
		 * Initialize.
		 */
		zip->bcj2_prevByte = 0;
		for (i = 0;
		    i < sizeof(zip->bcj2_p) / sizeof(zip->bcj2_p[0]); i++)
			zip->bcj2_p[i] = kBitModelTotal >> 1;
		RC_INIT2;
		zip->bcj_state = 1;
	}

	/*
	 * Gather the odd bytes of a previous call.
	 */
	for (i = 0; zip->odd_bcj_size > 0 && outPos < outSize; i++) {
		outBuf[outPos++] = zip->odd_bcj[i];
		zip->odd_bcj_size--;
	}

	if (outSize == 0) {
		zip->bcj2_outPos += outPos;
		return (outPos);
	}

	for (;;) {
		uint8_t b;
		CProb *prob;
		uint32_t bound;
		uint32_t ttt;

		size_t limit = size0 - inPos;
		if (outSize - outPos < limit)
			limit = outSize - outPos;

		if (zip->bcj_state == 1) {
			while (limit != 0) {
				uint8_t bb = buf0[inPos];
				outBuf[outPos++] = bb;
				if (IsJ(zip->bcj2_prevByte, bb)) {
					zip->bcj_state = 2;
					break;
				}
				inPos++;
				zip->bcj2_prevByte = bb;
				limit--;
			}
		}

		if (limit == 0 || outPos == outSize)
			break;
		zip->bcj_state = 1;

		b = buf0[inPos++];

		if (b == 0xE8)
			prob = zip->bcj2_p + zip->bcj2_prevByte;
		else if (b == 0xE9)
			prob = zip->bcj2_p + 256;
		else
			prob = zip->bcj2_p + 257;

		IF_BIT_0(prob) {
			UPDATE_0(prob)
			zip->bcj2_prevByte = b;
		} else {
			uint32_t dest;
			const uint8_t *v;
			uint8_t out[4];

			UPDATE_1(prob)
			if (b == 0xE8) {
				v = buf1;
				if (size1 < 4)
					return SZ_ERROR_DATA;
				buf1 += 4;
				size1 -= 4;
			} else {
				v = buf2;
				if (size2 < 4)
					return SZ_ERROR_DATA;
				buf2 += 4;
				size2 -= 4;
			}
			dest = (((uint32_t)v[0] << 24) |
			    ((uint32_t)v[1] << 16) |
			    ((uint32_t)v[2] << 8) |
			    ((uint32_t)v[3])) -
			    ((uint32_t)zip->bcj2_outPos + (uint32_t)outPos + 4);
			out[0] = (uint8_t)dest;
			out[1] = (uint8_t)(dest >> 8);
			out[2] = (uint8_t)(dest >> 16);
			out[3] = zip->bcj2_prevByte = (uint8_t)(dest >> 24);

			for (i = 0; i < 4 && outPos < outSize; i++)
				outBuf[outPos++] = out[i];
			if (i < 4) {
				/*
				 * Save odd bytes which we could not add into
				 * the output buffer because of out of space.
				 */
				zip->odd_bcj_size = 4 -i;
				for (; i < 4; i++) {
					j = i - 4 + (unsigned)zip->odd_bcj_size;
					zip->odd_bcj[j] = out[i];
				}
				break;
			}
		}
	}
	zip->tmp_stream_bytes_remaining -= inPos;
	zip->sub_stream_bytes_remaining[0] = size1;
	zip->sub_stream_bytes_remaining[1] = size2;
	zip->sub_stream_bytes_remaining[2] = bufferLim - buffer;
	zip->bcj2_outPos += outPos;

	return ((ssize_t)outPos);
}
