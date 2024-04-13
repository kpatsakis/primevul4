static int tg3_request_firmware(struct tg3 *tp)
{
	const __be32 *fw_data;

	if (request_firmware(&tp->fw, tp->fw_needed, &tp->pdev->dev)) {
		netdev_err(tp->dev, "Failed to load firmware \"%s\"\n",
			   tp->fw_needed);
		return -ENOENT;
	}

	fw_data = (void *)tp->fw->data;

	/* Firmware blob starts with version numbers, followed by
	 * start address and _full_ length including BSS sections
	 * (which must be longer than the actual data, of course
	 */

	tp->fw_len = be32_to_cpu(fw_data[2]);	/* includes bss */
	if (tp->fw_len < (tp->fw->size - 12)) {
		netdev_err(tp->dev, "bogus length %d in \"%s\"\n",
			   tp->fw_len, tp->fw_needed);
		release_firmware(tp->fw);
		tp->fw = NULL;
		return -EINVAL;
	}

	/* We no longer need firmware; we have it. */
	tp->fw_needed = NULL;
	return 0;
}
