int _yr_emit_inst(
    RE_EMIT_CONTEXT* emit_context,
    uint8_t opcode,
    uint8_t** instruction_addr,
    int* code_size)
{
  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &opcode,
      sizeof(uint8_t),
      (void**) instruction_addr));

  *code_size = sizeof(uint8_t);

  return ERROR_SUCCESS;
}
