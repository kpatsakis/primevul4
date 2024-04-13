onig_print_statistics(FILE* f)
{
  int r;
  int i;

  r = fprintf(f, "   count      prev        time\n");
  if (r < 0) return -1;

  for (i = 0; OnigOpInfo[i].opcode >= 0; i++) {
    r = fprintf(f, "%8d: %8d: %10ld: %s\n",
                OpCounter[i], OpPrevCounter[i], OpTime[i], OnigOpInfo[i].name);
    if (r < 0) return -1;
  }
  r = fprintf(f, "\nmax stack depth: %d\n", MaxStackDepth);
  if (r < 0) return -1;

  return 0;
}
