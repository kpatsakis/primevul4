setup_op_new (SetupOpType type)
{
  SetupOp *op = xcalloc (sizeof (SetupOp));

  op->type = type;
  op->fd = -1;
  op->flags = 0;
  if (last_op != NULL)
    last_op->next = op;
  else
    ops = op;

  last_op = op;
  return op;
}
