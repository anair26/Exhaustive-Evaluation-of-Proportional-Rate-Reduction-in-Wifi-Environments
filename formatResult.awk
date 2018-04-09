BEGIN {ssthresh=0;bytesInFlight=0;enter=0;printf("ssthresh\tbytesInFlight\tcWndAtTheEnd\tTimeSpentInRecovery\n");}
{
if($1 == "[node")
  {
    if($2 == "0]")
      {
        if($3=="Enter:")
          {
            enter = 1;
            ssthresh = $4;
            bytesInFlight = $5
          }
        if($3=="RTODuringRecovery")
          {
            enter = 0;
            printf("%d\t\t%d\t\t%d\t\t%f\n", ssthresh, bytesInFlight, $5, $6)
          }
        if($3=="Exit:" && enter==1)
          {
            printf("%d\t\t%d\t\t%d\t\t%f\n", ssthresh, bytesInFlight, $5, $6)
            enter = 0;
          }
      }
  }
}
END {
}
