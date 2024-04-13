static void vfio_pci_probe_mmaps(struct vfio_pci_device *vdev)
{
	struct resource *res;
	int bar;
	struct vfio_pci_dummy_resource *dummy_res;

	INIT_LIST_HEAD(&vdev->dummy_resources_list);

	for (bar = PCI_STD_RESOURCES; bar <= PCI_STD_RESOURCE_END; bar++) {
		res = vdev->pdev->resource + bar;

		if (!IS_ENABLED(CONFIG_VFIO_PCI_MMAP))
			goto no_mmap;

		if (!(res->flags & IORESOURCE_MEM))
			goto no_mmap;

		/*
		 * The PCI core shouldn't set up a resource with a
		 * type but zero size. But there may be bugs that
		 * cause us to do that.
		 */
		if (!resource_size(res))
			goto no_mmap;

		if (resource_size(res) >= PAGE_SIZE) {
			vdev->bar_mmap_supported[bar] = true;
			continue;
		}

		if (!(res->start & ~PAGE_MASK)) {
			/*
			 * Add a dummy resource to reserve the remainder
			 * of the exclusive page in case that hot-add
			 * device's bar is assigned into it.
			 */
			dummy_res = kzalloc(sizeof(*dummy_res), GFP_KERNEL);
			if (dummy_res == NULL)
				goto no_mmap;

			dummy_res->resource.name = "vfio sub-page reserved";
			dummy_res->resource.start = res->end + 1;
			dummy_res->resource.end = res->start + PAGE_SIZE - 1;
			dummy_res->resource.flags = res->flags;
			if (request_resource(res->parent,
						&dummy_res->resource)) {
				kfree(dummy_res);
				goto no_mmap;
			}
			dummy_res->index = bar;
			list_add(&dummy_res->res_next,
					&vdev->dummy_resources_list);
			vdev->bar_mmap_supported[bar] = true;
			continue;
		}
		/*
		 * Here we don't handle the case when the BAR is not page
		 * aligned because we can't expect the BAR will be
		 * assigned into the same location in a page in guest
		 * when we passthrough the BAR. And it's hard to access
		 * this BAR in userspace because we have no way to get
		 * the BAR's location in a page.
		 */
no_mmap:
		vdev->bar_mmap_supported[bar] = false;
	}
}
