json_t * user_auth_scheme_module_load(struct config_module * config) {
  UNUSED(config);
  return json_pack("{si ss ss ss }",
                   "result", G_OK,
                   "name", "webauthn",
                   "display_name", "WebAuthn",
                   "description", "WebAuthn scheme module");
}