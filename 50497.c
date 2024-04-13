static int vfio_pci_set_ctx_trigger_single(struct eventfd_ctx **ctx,
					   unsigned int count, uint32_t flags,
					   void *data)
{
	/* DATA_NONE/DATA_BOOL enables loopback testing */
	if (flags & VFIO_IRQ_SET_DATA_NONE) {
		if (*ctx) {
			if (count) {
				eventfd_signal(*ctx, 1);
			} else {
				eventfd_ctx_put(*ctx);
				*ctx = NULL;
			}
			return 0;
		}
	} else if (flags & VFIO_IRQ_SET_DATA_BOOL) {
		uint8_t trigger;

		if (!count)
			return -EINVAL;

		trigger = *(uint8_t *)data;
		if (trigger && *ctx)
			eventfd_signal(*ctx, 1);

		return 0;
	} else if (flags & VFIO_IRQ_SET_DATA_EVENTFD) {
		int32_t fd;

		if (!count)
			return -EINVAL;

		fd = *(int32_t *)data;
		if (fd == -1) {
			if (*ctx)
				eventfd_ctx_put(*ctx);
			*ctx = NULL;
		} else if (fd >= 0) {
			struct eventfd_ctx *efdctx;

			efdctx = eventfd_ctx_fdget(fd);
			if (IS_ERR(efdctx))
				return PTR_ERR(efdctx);

			if (*ctx)
				eventfd_ctx_put(*ctx);

			*ctx = efdctx;
		}
		return 0;
	}

	return -EINVAL;
}
