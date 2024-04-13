rend_service_add_filenames_to_list(smartlist_t *lst, const rend_service_t *s)
{
  tor_assert(lst);
  tor_assert(s);
  tor_assert(s->directory);
  smartlist_add(lst, rend_service_path(s, private_key_fname));
  smartlist_add(lst, rend_service_path(s, hostname_fname));
  smartlist_add(lst, rend_service_path(s, client_keys_fname));
  smartlist_add(lst, rend_service_sos_poison_path(s));
}
