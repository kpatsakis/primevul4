static ssize_t show_tabletExecute(struct device *dev, struct device_attribute *attr, char *buf)
{
	/* There is nothing useful to display, so a one-line manual
	 * is in order...
	 */
	return snprintf(buf, PAGE_SIZE,
			"Write anything to this file to program your tablet.\n");
}
