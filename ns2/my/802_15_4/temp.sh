rootY="YUnitText: "
output_param=("Throughput(kbps)" "Delay(s)" "Delivery Ratio" "Drop Ratio" "Energy Consumption(J)")
rootX="XUnitText: "
input_param=("Number of Nodes" "Number of Flows" "Number of Packets per second" "Coverage")
commonEnd="\\n\"\"\\n0 0.0\\n"

graph_root_directory="graph/"
nodeGraphs=("node_throughput.xgr" "node_delay.xgr" "node_delRate.xgr" "node_dropRate.xgr" "node_Energy.xgr")
flowGraphs=("flow_throughput.xgr" "flow_delay.xgr" "flow_delRate.xgr" "flow_dropRate.xgr" "flow_Energy.xgr")
ppsGraphs=("pps_throughput.xgr" "pps_delay.xgr" "pps_delRate.xgr" "pps_dropRate.xgr" "pps_Energy.xgr")
coverageGraphs=("coverage_throughput.xgr" "coverage_delay.xgr" "coverage_delRate.xgr" "coverage_dropRate.xgr" "coverage_Energy.xgr")

i=0
while [ $i -lt 5 ]
do
nodeGraphs[$i]=$graph_root_directory${nodeGraphs[$i]}
flowGraphs[$i]=$graph_root_directory${flowGraphs[$i]}
ppsGraphs[$i]=$graph_root_directory${ppsGraphs[$i]}
coverageGraphs[$i]=$graph_root_directory${coverageGraphs[$i]}

printf "$rootY${output_param[$i]}\n$rootX${input_param[0]}$commonEnd" > ${nodeGraphs[$i]}
printf "$rootY${output_param[$i]}\n$rootX${input_param[1]}$commonEnd" > ${flowGraphs[$i]}
printf "$rootY${output_param[$i]}\n$rootX${input_param[2]}$commonEnd" > ${ppsGraphs[$i]}
printf "$rootY${output_param[$i]}\n$rootX${input_param[3]}$commonEnd" > ${coverageGraphs[$i]}

i=`expr $i + 1`
done
