find_intro_circuit(rend_intro_point_t *intro, const char *pk_digest)
{
  origin_circuit_t *circ = NULL;

  tor_assert(intro);
  while ((circ = circuit_get_next_by_pk_and_purpose(circ,
                         (uint8_t *) pk_digest, CIRCUIT_PURPOSE_S_INTRO))) {
    if (tor_memeq(circ->build_state->chosen_exit->identity_digest,
                intro->extend_info->identity_digest, DIGEST_LEN) &&
        circ->rend_data) {
      return circ;
    }
  }

  circ = NULL;
  while ((circ = circuit_get_next_by_pk_and_purpose(circ,
                         (uint8_t *) pk_digest,
                         CIRCUIT_PURPOSE_S_ESTABLISH_INTRO))) {
    if (tor_memeq(circ->build_state->chosen_exit->identity_digest,
                intro->extend_info->identity_digest, DIGEST_LEN) &&
        circ->rend_data) {
      return circ;
    }
  }
  return NULL;
}
