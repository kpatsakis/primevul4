int CLASS median4 (int *p)
{
  int min, max, sum, i;

  min = max = sum = p[0];
  for (i=1; i < 4; i++) {
    sum += p[i];
    if (min > p[i]) min = p[i];
    if (max < p[i]) max = p[i];
  }
  return (sum - min - max) >> 1;
}
