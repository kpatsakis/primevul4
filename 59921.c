rend_services_add_filenames_to_lists(smartlist_t *open_lst,
                                     smartlist_t *stat_lst)
{
  if (!rend_service_list)
    return;
  SMARTLIST_FOREACH_BEGIN(rend_service_list, rend_service_t *, s) {
    if (!rend_service_is_ephemeral(s)) {
      rend_service_add_filenames_to_list(open_lst, s);
      smartlist_add_strdup(stat_lst, s->directory);
    }
  } SMARTLIST_FOREACH_END(s);
}
