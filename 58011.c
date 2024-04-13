int _yr_emit_split(
    RE_EMIT_CONTEXT* emit_context,
    uint8_t opcode,
    int16_t argument,
    uint8_t** instruction_addr,
    int16_t** argument_addr,
    int* code_size)
{
  assert(opcode == RE_OPCODE_SPLIT_A || opcode == RE_OPCODE_SPLIT_B);

  if (emit_context->next_split_id == RE_MAX_SPLIT_ID)
    return ERROR_REGULAR_EXPRESSION_TOO_COMPLEX;

  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &opcode,
      sizeof(uint8_t),
      (void**) instruction_addr));

  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &emit_context->next_split_id,
      sizeof(RE_SPLIT_ID_TYPE),
      NULL));

  emit_context->next_split_id++;

  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &argument,
      sizeof(int16_t),
      (void**) argument_addr));

  *code_size = sizeof(uint8_t) + sizeof(RE_SPLIT_ID_TYPE) + sizeof(int16_t);

  return ERROR_SUCCESS;
}
