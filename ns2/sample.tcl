set ns [new Simulator]
set nf [open test.tr w]
$ns trace-all $nf
$ns rtproto DV
set n0 [$ns node]
set n1 [$ns node]
$ns duplex-link $n0 $n1 1Mb 10ms DropTail
set cbr0 [new Application/Traffic/CBR]
$cbr0 set packetSize_ 500
$cbr0 set interval_ 0.005
set udp0 [new Agent/UDP]
$ns attach_agent $n0 $udp0
$cbr0 attach_agent $udp0
set null0 [new Agent/Null]
$ns attach_agent $n1 $null0
$ns at 0.1 "$cbr0 start"
$ns at 5.0 "$cbr0 stop"

proc finish{} {
	global ns nf
	$ns flush_trace
	close $nf
	exit 0
}

$ns at 5.5 "finish"
$ns run

