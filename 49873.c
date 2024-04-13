static int hash_process_data(struct hash_device_data *device_data,
			     struct hash_ctx *ctx, struct hash_req_ctx *req_ctx,
			     int msg_length, u8 *data_buffer, u8 *buffer,
			     u8 *index)
{
	int ret = 0;
	u32 count;

	do {
		if ((*index + msg_length) < HASH_BLOCK_SIZE) {
			for (count = 0; count < msg_length; count++) {
				buffer[*index + count] =
					*(data_buffer + count);
			}
			*index += msg_length;
			msg_length = 0;
		} else {
			if (req_ctx->updated) {
				ret = hash_resume_state(device_data,
						&device_data->state);
				memmove(req_ctx->state.buffer,
					device_data->state.buffer,
					HASH_BLOCK_SIZE / sizeof(u32));
				if (ret) {
					dev_err(device_data->dev,
						"%s: hash_resume_state() failed!\n",
						__func__);
					goto out;
				}
			} else {
				ret = init_hash_hw(device_data, ctx);
				if (ret) {
					dev_err(device_data->dev,
						"%s: init_hash_hw() failed!\n",
						__func__);
					goto out;
				}
				req_ctx->updated = 1;
			}
			/*
			 * If 'data_buffer' is four byte aligned and
			 * local buffer does not have any data, we can
			 * write data directly from 'data_buffer' to
			 * HW peripheral, otherwise we first copy data
			 * to a local buffer
			 */
			if ((0 == (((u32)data_buffer) % 4)) &&
			    (0 == *index))
				hash_processblock(device_data,
						  (const u32 *)data_buffer,
						  HASH_BLOCK_SIZE);
			else {
				for (count = 0;
				     count < (u32)(HASH_BLOCK_SIZE - *index);
				     count++) {
					buffer[*index + count] =
						*(data_buffer + count);
				}
				hash_processblock(device_data,
						  (const u32 *)buffer,
						  HASH_BLOCK_SIZE);
			}
			hash_incrementlength(req_ctx, HASH_BLOCK_SIZE);
			data_buffer += (HASH_BLOCK_SIZE - *index);

			msg_length -= (HASH_BLOCK_SIZE - *index);
			*index = 0;

			ret = hash_save_state(device_data,
					&device_data->state);

			memmove(device_data->state.buffer,
				req_ctx->state.buffer,
				HASH_BLOCK_SIZE / sizeof(u32));
			if (ret) {
				dev_err(device_data->dev, "%s: hash_save_state() failed!\n",
					__func__);
				goto out;
			}
		}
	} while (msg_length != 0);
out:

	return ret;
}
