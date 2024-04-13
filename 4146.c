json_t * user_auth_scheme_module_identify(struct config_module * config, const struct _u_request * http_request, json_t * j_scheme_data, void * cls) {
  UNUSED(config);
  UNUSED(http_request);
  UNUSED(j_scheme_data);
  UNUSED(cls);
  return json_pack("{si}", "result", G_ERROR_UNAUTHORIZED);
}