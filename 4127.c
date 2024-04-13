json_t * user_auth_scheme_module_init(struct config_module * config, json_t * j_parameters, const char * mod_name, void ** cls) {
  UNUSED(config);
  json_t * j_result = is_scheme_parameters_valid(j_parameters), * j_element = NULL, * j_return;
  size_t index = 0;
  char * message;

  if (check_result_value(j_result, G_OK)) {
    *cls = json_pack("{sO sO sO sO sI sI sO ss so sO sO sO sO sO sO sO ss s[]}",
                     "challenge-length", json_object_get(j_parameters, "challenge-length"),
                     "rp-origin", json_object_get(j_parameters, "rp-origin"),
                     "credential-expiration", json_object_get(j_parameters, "credential-expiration"),
                     "credential-assertion", json_object_get(j_parameters, "credential-assertion"),
                     "ctsProfileMatch", json_object_get(j_parameters, "ctsProfileMatch")!=NULL?json_integer_value(json_object_get(j_parameters, "ctsProfileMatch")):-1,
                     "basicIntegrity", json_object_get(j_parameters, "basicIntegrity")!=NULL?json_integer_value(json_object_get(j_parameters, "basicIntegrity")):-1,
                     "session-mandatory", json_object_get(j_parameters, "session-mandatory")!=NULL?json_object_get(j_parameters, "session-mandatory"):json_true(),
                     "seed", !json_string_length(json_object_get(j_parameters, "seed"))?"":json_string_value(json_object_get(j_parameters, "seed")),
                     "fmt", json_object_get(j_parameters, "fmt")!=NULL?json_deep_copy(json_object_get(j_parameters, "fmt")):json_pack("{sosososososo}", "packed", json_true(), "tpm", json_true(), "android-key", json_true(), "android-safetynet", json_true(), "fido-u2f", json_true(), "none", json_true()),
                     "force-fmt-none", json_object_get(j_parameters, "force-fmt-none")!=NULL?json_object_get(j_parameters, "force-fmt-none"):json_false(),
                     "google-root-ca-r2", json_string_length(json_object_get(j_parameters, "google-root-ca-r2"))?json_object_get(j_parameters, "google-root-ca-r2"):json_null(),
                     "google-root-ca-r2-content", json_object_get(j_parameters, "google-root-ca-r2-content")!=NULL?json_object_get(j_parameters, "google-root-ca-r2-content"):json_null(),
                     "apple-root-ca", json_string_length(json_object_get(j_parameters, "apple-root-ca"))?json_object_get(j_parameters, "apple-root-ca"):json_null(),
                     "apple-root-ca-content", json_object_get(j_parameters, "apple-root-ca-content")!=NULL?json_object_get(j_parameters, "apple-root-ca-content"):json_null(),
                     "root-ca-list", json_array_size(json_object_get(j_parameters, "root-ca-list"))?json_object_get(j_parameters, "root-ca-list"):json_null(),
                     "root-ca-array", json_object_get(j_parameters, "root-ca-array")!=NULL?json_object_get(j_parameters, "root-ca-array"):json_null(),
                     "mod_name", mod_name,
                     "pubKey-cred-params");
    json_array_foreach(json_object_get(j_parameters, "pubKey-cred-params"), index, j_element) {
      json_array_append_new(json_object_get((json_t *)*cls, "pubKey-cred-params"), json_pack("{sssO}", "type", "public-key", "alg", j_element));
    }
    j_return = json_pack("{si}", "result", G_OK);
  } else if (check_result_value(j_result, G_ERROR_PARAM)) {
    message = json_dumps(json_object_get(j_result, "error"), JSON_COMPACT);
    y_log_message(Y_LOG_LEVEL_ERROR, "user_auth_scheme_module_init webauthn - Error input parameters: %s", message);
    o_free(message);
    j_return = json_pack("{sisO}", "result", G_ERROR_PARAM, "error", json_object_get(j_result, "error"));
  } else {
    j_return = json_pack("{si}", "result", G_ERROR);
  }
  json_decref(j_result);
  return j_return;
}