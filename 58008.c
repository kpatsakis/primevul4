int _yr_emit_inst_arg_int16(
    RE_EMIT_CONTEXT* emit_context,
    uint8_t opcode,
    int16_t argument,
    uint8_t** instruction_addr,
    int16_t** argument_addr,
    int* code_size)
{
  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &opcode,
      sizeof(uint8_t),
      (void**) instruction_addr));

  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &argument,
      sizeof(int16_t),
      (void**) argument_addr));

  *code_size = sizeof(uint8_t) + sizeof(int16_t);

  return ERROR_SUCCESS;
}
