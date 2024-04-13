static int ecryptfs_copy_filename(char **copied_name, size_t *copied_name_size,
				  const char *name, size_t name_size)
{
	int rc = 0;

	(*copied_name) = kmalloc((name_size + 1), GFP_KERNEL);
	if (!(*copied_name)) {
		rc = -ENOMEM;
		goto out;
	}
	memcpy((void *)(*copied_name), (void *)name, name_size);
	(*copied_name)[(name_size)] = '\0';	/* Only for convenience
						 * in printing out the
						 * string in debug
						 * messages */
	(*copied_name_size) = name_size;
out:
	return rc;
}
