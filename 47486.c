static int checksoftirq(int force){
  int cpu=0;

  for(;;){
    switch(checksoftirq2(force,cpu)){
    case -1:
      return -1;
    case 1:
      cpu++;
      break;
    case 0:
    default:
      return 0;
    }
  }
  return 0;
}
