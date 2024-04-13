reset_state_machine (int is_stmt)
{
  state_machine_regs.address = 0;
  state_machine_regs.view = 0;
  state_machine_regs.op_index = 0;
  state_machine_regs.file = 1;
  state_machine_regs.line = 1;
  state_machine_regs.column = 0;
  state_machine_regs.is_stmt = is_stmt;
  state_machine_regs.basic_block = 0;
  state_machine_regs.end_sequence = 0;
  state_machine_regs.last_file_entry = 0;
}