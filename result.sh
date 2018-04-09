NS_LOG="TcpSocketBase=warn" ./waf --run "scratch/PRR-example --recoveryType=ns3::ClassicRecovery" > result 2>&1
awk -f formatResult.awk result > ClassicResult

NS_LOG="TcpSocketBase=warn" ./waf --run "scratch/PRR-example --recoveryType=ns3::PrrRecovery --reductionBound=SSRB" > result 2>&1
awk -f formatResult.awk result > PrrSSRBResult

NS_LOG="TcpSocketBase=warn" ./waf --run "scratch/PRR-example --recoveryType=ns3::PrrRecovery --reductionBound=CRB" > result 2>&1
awk -f formatResult.awk result > PrrCRBResult

rm -rf result
