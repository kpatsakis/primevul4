int _yr_emit_inst_arg_uint32(
    RE_EMIT_CONTEXT* emit_context,
    uint8_t opcode,
    uint32_t argument,
    uint8_t** instruction_addr,
    uint32_t** argument_addr,
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
      sizeof(uint32_t),
      (void**) argument_addr));

  *code_size = sizeof(uint8_t) + sizeof(uint32_t);

  return ERROR_SUCCESS;
}
