static int msix_sparse_mmap_cap(struct vfio_pci_device *vdev,
				struct vfio_info_cap *caps)
{
	struct vfio_info_cap_header *header;
	struct vfio_region_info_cap_sparse_mmap *sparse;
	size_t end, size;
	int nr_areas = 2, i = 0;

	end = pci_resource_len(vdev->pdev, vdev->msix_bar);

	/* If MSI-X table is aligned to the start or end, only one area */
	if (((vdev->msix_offset & PAGE_MASK) == 0) ||
	    (PAGE_ALIGN(vdev->msix_offset + vdev->msix_size) >= end))
		nr_areas = 1;

	size = sizeof(*sparse) + (nr_areas * sizeof(*sparse->areas));

	header = vfio_info_cap_add(caps, size,
				   VFIO_REGION_INFO_CAP_SPARSE_MMAP, 1);
	if (IS_ERR(header))
		return PTR_ERR(header);

	sparse = container_of(header,
			      struct vfio_region_info_cap_sparse_mmap, header);
	sparse->nr_areas = nr_areas;

	if (vdev->msix_offset & PAGE_MASK) {
		sparse->areas[i].offset = 0;
		sparse->areas[i].size = vdev->msix_offset & PAGE_MASK;
		i++;
	}

	if (PAGE_ALIGN(vdev->msix_offset + vdev->msix_size) < end) {
		sparse->areas[i].offset = PAGE_ALIGN(vdev->msix_offset +
						     vdev->msix_size);
		sparse->areas[i].size = end - sparse->areas[i].offset;
		i++;
	}

	return 0;
}
