acpi_ns_get_node(struct acpi_namespace_node *prefix_node,
		 const char *pathname,
		 u32 flags, struct acpi_namespace_node **return_node)
{
	acpi_status status;

	ACPI_FUNCTION_TRACE_PTR(ns_get_node, ACPI_CAST_PTR(char, pathname));

	status = acpi_ut_acquire_mutex(ACPI_MTX_NAMESPACE);
	if (ACPI_FAILURE(status)) {
		return_ACPI_STATUS(status);
	}

	status = acpi_ns_get_node_unlocked(prefix_node, pathname,
					   flags, return_node);

	(void)acpi_ut_release_mutex(ACPI_MTX_NAMESPACE);
	return_ACPI_STATUS(status);
}
