static inline double Permutate(const ssize_t n,const ssize_t k)
{
  double
    r;

  register ssize_t
    i;

  r=1.0;
  for (i=k+1; i <= n; i++)
    r*=i;
  for (i=1; i <= (n-k); i++)
    r/=i;
  return(r);
}
