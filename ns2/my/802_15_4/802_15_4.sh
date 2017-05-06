nodes=(20 40 60 80 100)
flows=(10 20 30 40 50)
packets_per_second=(100 200 300 400 500)
speed=(5 10 15 20 25)

iteration=5

###varying nodes-------------------------------
echo "------------------------------------------"
echo "Simulating with different number of nodes"
echo "------------------------------------------"

i=0
while [ $i -lt $iteration ]
do
params="${nodes[$i]} ${flows[0]} ${packets_per_second[0]} ${speed[0]}"
out="${nodes[$i]}_${flows[0]}_${packets_per_second[0]}_${speed[0]}"

ns 802_15_4.tcl $params
awk -f 802_15_4.awk trace_802_15_4.tr > out/"$out".out
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

ns 802_15_4.tcl $params
awk -f 802_15_4.awk trace_802_15_4.tr > out/"$out".out
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

ns 802_15_4.tcl $params
awk -f 802_15_4.awk trace_802_15_4.tr > out/"$out".out
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

ns 802_15_4.tcl $params
awk -f 802_15_4.awk trace_802_15_4.tr > out/"$out".out
i=`expr $i + 1`
done



#a=0
#while [ $a -lt 10 ]
#do
#   echo $a
#   a=`expr $a + 1`
#done
