puts "Parameters: [lindex $argv 0] [lindex $argv 1] [lindex $argv 2] [lindex $argv 3]\n"
set out "[lindex $argv 0]_[lindex $argv 1]_[lindex $argv 2]_[lindex $argv 3]"
################################################################802.11 in Grid topology with cross folw
set cbr_size 8 ; #[lindex $argv 2]; #4,8,16,32,64
set cbr_rate 11Mb

 ;# ?????? 1 for 1 packets per second and 0.1 for 10 packets per second
#set cbr_interval 0.00005 ; #[expr 1/[lindex $argv 2]] ;# ?????? 1 for 1 packets per second and 0.1 for 10 packets per second

set num_node [lindex $argv 0]
set num_random_flow [lindex $argv 1]
set cbr_pckt_per_sec [lindex $argv 2]
set coverage_multiplier [lindex $argv 3]

set cbr_interval [expr 1.0/$cbr_pckt_per_sec]

set x_dim [expr $coverage_multiplier*15.0] ; #[lindex $argv 1]
set y_dim [expr $coverage_multiplier*15.0] ; #[lindex $argv 1]

set dist(5m)  7.69113e-06
set dist(9m)  2.37381e-06
set dist(10m) 1.92278e-06
set dist(11m) 1.58908e-06
set dist(12m) 1.33527e-06
set dist(13m) 1.13774e-06
set dist(14m) 9.81011e-07
set dist(15m) 8.54570e-07
set dist(16m) 7.51087e-07
set dist(20m) 4.80696e-07
set dist(25m) 3.07645e-07
set dist(30m) 2.13643e-07
set dist(35m) 1.56962e-07
set dist(40m) 1.20174e-07
Phy/WirelessPhy set CSThresh_ $dist(15m)
Phy/WirelessPhy set RXThresh_ $dist(15m)
Phy/WirelessPhy set TXThresh_ $dist(15m)

set time_duration 30 ; #[lindex $argv 5] ;#50
set time_duration 5 ; #[lindex $argv 5] ;#50
set start_time 5 ;#100
set parallel_start_gap 0.0
set cross_start_gap 0.0

#############################################################ENERGY PARAMETERS
set val(energymodel_11)    EnergyModel     ;
set val(initialenergy_11)  1000            ;# Initial energy in Joules

set val(idlepower_11) 869.4e-3			;#LEAP (802.11g) 
set val(rxpower_11) 1560.6e-3			;#LEAP (802.11g)
set val(txpower_11) 1679.4e-3			;#LEAP (802.11g)
set val(sleeppower_11) 37.8e-3			;#LEAP (802.11g)
set val(transitionpower_11) 176.695e-3		;#LEAP (802.11g)	??????????????????????????????/
set val(transitiontime_11) 2.36			;#LEAP (802.11g)

#set val(idlepower_11) 900e-3			;#Stargate (802.11b) 
#set val(rxpower_11) 925e-3			;#Stargate (802.11b)
#set val(txpower_11) 1425e-3			;#Stargate (802.11b)
#set val(sleeppower_11) 300e-3			;#Stargate (802.11b)
#set val(transitionpower_11) 200e-3		;#Stargate (802.11b)	??????????????????????????????/
#set val(transitiontime_11) 3			;#Stargate (802.11b)

Mac/802_15_4 set dataRate_ 11Mb

set extra_time 10 ;#10
set tcp_src Agent/UDP
set tcp_sink Agent/Null




#	http://research.cens.ucla.edu/people/estrin/resources/conferences/2007may-Stathopoulos-Lukac-Dual_Radio.pdf


set val(chan) Channel/WirelessChannel ;# channel type
set val(prop) Propagation/TwoRayGround ;# radio-propagation model
#set val(prop) Propagation/FreeSpace ;# radio-propagation model
set val(netif) Phy/WirelessPhy/802_15_4 ;# network interface type
set val(mac) Mac/802_15_4 ;# MAC type
#set val(mac) Mac/802_11;# MAC type
set val(ifq) Queue/DropTail/PriQueue ;# interface queue type
set val(ll) LL ;# link layer type
set val(ant) Antenna/OmniAntenna ;# antenna model
set val(ifqlen) 50 ;# max packet in ifq
set val(rp) AODV ; #[lindex $argv 4] ;# routing protocol

Mac/802_15_4 set syncFlag_ 1

Mac/802_15_4 set dutyCycle_ cbr_interval

set nm nam_802_15_4.nam
set tr trace_802_15_4.tr
set topo_file ./topo/$out.topo

# 
# Initialize ns
#
set ns_ [new Simulator]

set tracefd [open $tr w]
$ns_ trace-all $tracefd


set namtrace [open $nm w]

set topofile [open $topo_file "w"]

# set up topography object
set topo       [new Topography]
$topo load_flatgrid $x_dim $y_dim


create-god [expr $num_node ]




$ns_ node-config -adhocRouting $val(rp) -llType $val(ll) \
     -macType $val(mac)  -ifqType $val(ifq) \
     -ifqLen $val(ifqlen) -antType $val(ant) \
     -propType $val(prop) -phyType $val(netif) \
     -channel  [new $val(chan)] -topoInstance $topo \
     -agentTrace ON -routerTrace OFF\
     -macTrace ON \
     -movementTrace OFF \
			 -energyModel $val(energymodel_11) \
			 -idlePower $val(idlepower_11) \
			 -rxPower $val(rxpower_11) \
			 -txPower $val(txpower_11) \
          		 -sleepPower $val(sleeppower_11) \
          		 -transitionPower $val(transitionpower_11) \
			 -transitionTime $val(transitiontime_11) \
			 -initialEnergy $val(initialenergy_11)

 

puts "start node creation"
for {set i 0} {$i < $num_node} {incr i} {
	set node_($i) [$ns_ node]
#	$node_($i) random-motion 0
}

#FULL CHNG

set i 0;
while {$i < $num_node } {


	set x_pos [expr 1+int(($x_dim-2)*0.1*rand())] ;#random settings
	set y_pos [expr 1+int(($y_dim-2)*0.1*rand())] ;#random settings
	set x_pos [expr int($x_dim*rand())] ;#random settings
	set y_pos [expr 1+int($y_dim*rand())] ;#random settings

	$node_($i) set X_ $x_pos;
	$node_($i) set Y_ $y_pos;
	$node_($i) set Z_ 0.0

	puts -nonewline $topofile "$i x: [$node_($i) set X_] y: [$node_($i) set Y_] \n"

    incr i;
}; 

puts "RANDOM topology"

puts "node creation complete"

#CHNG
for {set i 0} {$i < $num_random_flow} {incr i} { ;#sink
#    set udp_($i) [new Agent/UDP]
#    set null_($i) [new Agent/Null]

	set udp_($i) [new $tcp_src]
	$udp_($i) set class_ $i
	set null_($i) [new $tcp_sink]
	$udp_($i) set fid_ $i
	if { [expr $i%2] == 0} {
		$ns_ color $i Blue
	} else {
		$ns_ color $i Red
	}

} 

set r 0 ;
#######################################################################RANDOM FLOW
set rt $r
for {set i 1} {$i < [expr $num_random_flow+1]} {incr i} {
	set udp_node [expr int($num_node*rand())] ;# src node
	set null_node $udp_node
	while {$null_node==$udp_node} {
		set null_node [expr int($num_node*rand())] ;# dest node
	}
	$ns_ attach-agent $node_($udp_node) $udp_($rt)
  	$ns_ attach-agent $node_($null_node) $null_($rt)
	puts -nonewline $topofile "RANDOM:  Src: $udp_node Dest: $null_node\n"
	incr rt
} 

set rt $r
for {set i 1} {$i < [expr $num_random_flow+1]} {incr i} {
	$ns_ connect $udp_($rt) $null_($rt)
	incr rt
}
set rt $r
for {set i 1} {$i < [expr $num_random_flow+1]} {incr i} {
	set cbr_($rt) [new Application/Traffic/CBR]
	$cbr_($rt) set packetSize_ $cbr_size
	$cbr_($rt) set rate_ $cbr_rate
	$cbr_($rt) set interval_ $cbr_interval
	$cbr_($rt) attach-agent $udp_($rt)
	incr rt
} 

set rt $r
for {set i 1} {$i < [expr $num_random_flow+1]} {incr i} {
	$ns_ at [expr $start_time] "$cbr_($rt) start"
	incr rt
}


puts "flow creation complete"
##########################################################################END OF FLOW GENERATION

# Tell nodes when the simulation ends
#
for {set i 0} {$i < $num_node } {incr i} {
    $ns_ at [expr $start_time+$time_duration] "$node_($i) reset";
}
$ns_ at [expr $start_time+$time_duration +$extra_time] "finish"

$ns_ at [expr $start_time+$time_duration +$extra_time] "$ns_ nam-end-wireless [$ns_ now]; puts \"NS Exiting...\"; $ns_ halt"

$ns_ at [expr $start_time+$time_duration/2] "puts \"half of the simulation is finished\""
$ns_ at [expr $start_time+$time_duration] "puts \"end of simulation duration\""

proc finish {} {
	puts "finishing\n\n"
	global ns_ tracefd namtrace topofile nm

	$ns_ flush-trace
	close $tracefd
	close $namtrace
	close $topofile
     #   exec xgraph graph.xgr &
        exit 0
}


for {set i 0} {$i < [expr $num_node]  } { incr i} {
	$ns_ initial_node_pos $node_($i) 4
}

puts "Starting Simulation..."
$ns_ run 


