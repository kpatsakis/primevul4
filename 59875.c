rend_consider_descriptor_republication(void)
{
  int i;
  rend_service_t *service;

  if (!consider_republishing_rend_descriptors)
    return;
  consider_republishing_rend_descriptors = 0;

  if (!get_options()->PublishHidServDescriptors)
    return;

  for (i=0; i < smartlist_len(rend_service_list); ++i) {
    service = smartlist_get(rend_service_list, i);
    if (service->desc && !service->desc->all_uploads_performed) {
      /* If we failed in uploading a descriptor last time, try again *without*
       * updating the descriptor's contents. */
      upload_service_descriptor(service);
    }
  }
}
