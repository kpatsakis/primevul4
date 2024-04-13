static int is_a_member(int val,int *vals,int num_vals){
  int lokke;
  for(lokke=0;lokke<num_vals;lokke++)
    if(val==vals[lokke])
      return 1;
  return 0;
}
