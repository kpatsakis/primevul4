void CLASS nikon_3700()
{
  int bits, i;
  uchar dp[24];
  static const struct {
    int bits;
    char make[12], model[15];
  } table[] = {
    { 0x00, "PENTAX",  "Optio 33WR" },
    { 0x03, "NIKON",   "E3200" },
    { 0x32, "NIKON",   "E3700" },
    { 0x33, "OLYMPUS", "C740UZ" } };

  fseek (ifp, 3072, SEEK_SET);
  fread (dp, 1, 24, ifp);
  bits = (dp[8] & 3) << 4 | (dp[20] & 3);
  for (i=0; i < (int) sizeof table / (int) sizeof *table; i++)
    if (bits == table[i].bits) {
      strcpy (make,  table[i].make );
      strcpy (model, table[i].model);
    }
}
