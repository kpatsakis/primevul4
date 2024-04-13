int _yr_emit_inst_arg_uint8(
    RE_EMIT_CONTEXT* emit_context,
    uint8_t opcode,
    uint8_t argument,
    uint8_t** instruction_addr,
    uint8_t** argument_addr,
    size_t* code_size)
{
  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &opcode,
      sizeof(uint8_t),
      (void**) instruction_addr));

  FAIL_ON_ERROR(yr_arena_write_data(
      emit_context->arena,
      &argument,
      sizeof(uint8_t),
      (void**) argument_addr));

  *code_size = 2 * sizeof(uint8_t);

  return ERROR_SUCCESS;
}
