nodes=(20 40 60 80 100)
flows=(10 20 30 40 50)
packets_per_second=(100 200 300 400 500)
speed=(5 10 15 20 25)

iteration=5
outputGraph="graphs_802_11.ps"
outputGraph="graphs.ps"


#graph files initialization
rootY="YUnitText: "
output_param=("Throughput(kbps)" "Delay(s)" "Delivery Ratio" "Drop Ratio" "Energy Consumption(J)")
rootX="XUnitText: "
input_param=("Number of Nodes" "Number of Flows" "Number of Packets per second" "Speed")
commonEnd="\\n\"\"\\n0 0.0\\n"

graph_root_directory="graph/"
nodeGraphs=("node_throughput.xgr" "node_delay.xgr" "node_delRate.xgr" "node_dropRate.xgr" "node_Energy.xgr")
flowGraphs=("flow_throughput.xgr" "flow_delay.xgr" "flow_delRate.xgr" "flow_dropRate.xgr" "flow_Energy.xgr")
ppsGraphs=("pps_throughput.xgr" "pps_delay.xgr" "pps_delRate.xgr" "pps_dropRate.xgr" "pps_Energy.xgr")
speedGraphs=("speed_throughput.xgr" "speed_delay.xgr" "speed_delRate.xgr" "speed_dropRate.xgr" "speed_Energy.xgr")

i=0
while [ $i -lt 5 ]
do
nodeGraphs[$i]=$graph_root_directory${nodeGraphs[$i]}
flowGraphs[$i]=$graph_root_directory${flowGraphs[$i]}
ppsGraphs[$i]=$graph_root_directory${ppsGraphs[$i]}
speedGraphs[$i]=$graph_root_directory${speedGraphs[$i]}

printf "$rootY${output_param[$i]}\n$rootX${input_param[0]}$commonEnd" > ${nodeGraphs[$i]}
printf "$rootY${output_param[$i]}\n$rootX${input_param[1]}$commonEnd" > ${flowGraphs[$i]}
printf "$rootY${output_param[$i]}\n$rootX${input_param[2]}$commonEnd" > ${ppsGraphs[$i]}
printf "$rootY${output_param[$i]}\n$rootX${input_param[3]}$commonEnd" > ${speedGraphs[$i]}

i=`expr $i + 1`
done

#Graph files initialized successfully-----------------

#initial Round----------------------
echo "------------------------------------------"
echo "Simulating with the base values"
echo "------------------------------------------"

params="${nodes[0]} ${flows[0]} ${packets_per_second[0]} ${speed[0]}"
out="${nodes[0]}_${flows[0]}_${packets_per_second[0]}_${speed[0]}"

ns 802_11.tcl $params
awk -f 802_11.awk trace_802_11.tr > out/"$out".out

line_no=0
while read line
do
	printf "${nodes[0]} $line\n" >> ${nodeGraphs[$line_no]}
	printf "${flows[0]} $line\n" >> ${flowGraphs[$line_no]}
	printf "${packets_per_second[0]} $line\n" >> ${ppsGraphs[$line_no]}
	printf "${speed[0]} $line\n" >> ${speedGraphs[$line_no]}
	line_no=`expr $line_no + 1`
done < out/"$out".out

#end initial round------------


###varying nodes-------------------------------
echo "------------------------------------------"
echo "Simulating with different number of nodes"
echo "------------------------------------------"

i=1
while [ $i -lt $iteration ]
do
params="${nodes[$i]} ${flows[0]} ${packets_per_second[0]} ${speed[0]}"
out="${nodes[$i]}_${flows[0]}_${packets_per_second[0]}_${speed[0]}"

ns 802_11.tcl $params
awk -f 802_11.awk trace_802_11.tr > out/"$out".out

line_no=0
	while read line
	do
		printf "${nodes[$i]} $line\n" >> ${nodeGraphs[$line_no]}
		line_no=`expr $line_no + 1`
	done < out/"$out".out

i=`expr $i + 1`
done

###varying flows-------------------------------
echo "------------------------------------------"
echo "Simulating with different number of flows"
echo "------------------------------------------"
i=1
while [ $i -lt $iteration ]
do
params="${nodes[0]} ${flows[$i]} ${packets_per_second[0]} ${speed[0]}"
out="${nodes[0]}_${flows[$i]}_${packets_per_second[0]}_${speed[0]}"

ns 802_11.tcl $params
awk -f 802_11.awk trace_802_11.tr > out/"$out".out

line_no=0
	while read line
	do
		printf "${flows[$i]} $line\n" >> ${flowGraphs[$line_no]}
		line_no=`expr $line_no + 1`
	done < out/"$out".out

i=`expr $i + 1`
done

###varying packets per second-------------------------------
echo "------------------------------------------"
echo "Simulating with different rate of packets per second"
echo "------------------------------------------"
i=1
while [ $i -lt $iteration ]
do
params="${nodes[0]} ${flows[0]} ${packets_per_second[$i]} ${speed[0]}"
out="${nodes[0]}_${flows[0]}_${packets_per_second[$i]}_${speed[0]}"

ns 802_11.tcl $params
awk -f 802_11.awk trace_802_11.tr > out/"$out".out
line_no=0
	while read line
	do
		printf "${packets_per_second[$i]} $line\n" >> ${ppsGraphs[$line_no]}
		line_no=`expr $line_no + 1`
	done < out/"$out".out


i=`expr $i + 1`
done

###varying speed-------------------------------
echo "------------------------------------------"
echo "Simulating with different speeds"
echo "------------------------------------------"
i=1
while [ $i -lt $iteration ]
do
params="${nodes[0]} ${flows[0]} ${packets_per_second[0]} ${speed[$i]}"
out="${nodes[0]}_${flows[0]}_${packets_per_second[0]}_${speed[$i]}"

line_no=0
	while read line
	do
		printf "${speed[$i]} $line\n" >> ${speedGraphs[$line_no]}
		line_no=`expr $line_no + 1`
	done < out/"$out".out

ns 802_11.tcl $params
awk -f 802_11.awk trace_802_11.tr > out/"$out".out
i=`expr $i + 1`
done



#a=0
#while [ $a -lt 10 ]
#do
#   echo $a
#   a=`expr $a + 1`
#done

i=0
while [ $i -lt 5 ]
do

xgraph -device ps -o ${nodeGraphs[$i]}.ps ${nodeGraphs[$i]}
xgraph -device ps -o ${flowGraphs[$i]}.ps ${flowGraphs[$i]}
xgraph -device ps -o ${ppsGraphs[$i]}.ps ${ppsGraphs[$i]}
xgraph -device ps -o ${speedGraphs[$i]}.ps ${speedGraphs[$i]}

i=`expr $i + 1`
done


largeName=""
i=0
while [ $i -lt 5 ]
do 
	largeName="$largeName ${nodeGraphs[$i]}.ps"
i=`expr $i + 1`
done

i=0
while [ $i -lt 5 ]
do 
	largeName="$largeName ${flowGraphs[$i]}.ps"
i=`expr $i + 1`
done

i=0
while [ $i -lt 5 ]
do 
	largeName="$largeName ${ppsGraphs[$i]}.ps"
i=`expr $i + 1`
done

i=0
while [ $i -lt 5 ]
do 
	largeName="$largeName ${speedGraphs[$i]}.ps"
i=`expr $i + 1`
done

gs -dBATCH -dNOPAUSE -q -sDEVICE=ps2write -sOutputFile=$outputGraph $largeName
xdg-open $outputGraph
