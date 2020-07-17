
#############################model to be used for testing CADSees program########################
######################developed by Seyed Alireza Jalali (seyyed-jalali@aut.ac.ir)################
######################at Amirkabir University of Technology (OpenSees.aut.ac.ir)#################


logCommands -file testCommands2d 			;#We cannot have AutoCAD understand TCL commands while importing our model
											;#all we currently need to draw deformed model geometry in CAD will be
											;#generated using this command in a file named "testCommands.tcl"
											;#if OpenSees blames that "logCommands" is not recognized as a command,
											;#that is because you are not using the AUT version of OpenSees.exe (download from OpenSees.aut.ac.ir)
set nflrs 1
set nbays 1
set L 600.			;#bay length in cm
set H 300.			;#story height in cm
#W10X12 properties in cm
set Iz 2252.5
set Area 22.9

#steel material
set E 2.1e6

model basic -ndm 2 -ndf 3
set nodeList [list]							;#we will need this if we are combining all node disps in a single recorder file
for {set j 0} {$j <= $nflrs} {incr j} {
	for {set i 1} {$i <= [expr $nbays+1]} {incr i} {
		set tag [expr $j*10+$i]
		node $tag [expr ($i-1)*$L] [expr $j*$H]
		if {$j != 0} {
			lappend nodeList $tag
		}
	}
}
fixY 0. 1 1 1
geomTransf Linear 1
for {set j 1} {$j <= $nflrs} {incr j} {
	for {set i 1} {$i <= [expr $nbays+1]} {incr i} {
		set tag [expr 100+$j*10+$i]
		set iNode [expr ($j-1)*10+$i]
		set jNode [expr $j*10+$i]
		element elasticBeamColumn $tag $iNode $jNode $Area $E $Iz 1
	}
}
for {set j 1} {$j <= $nflrs} {incr j} {
	for {set i 1} {$i <= $nbays} {incr i} {
		set tag [expr 200+$j*10+$i]
		set iNode [expr $j*10+$i]
		set jNode [expr $j*10+$i+1]
		element elasticBeamColumn $tag $iNode $jNode $Area $E $Iz 1
	}
}
set sumFlrs [expr $nflrs*($nflrs+1)*0.5]
pattern Plain 1 Linear {
	for {set j 1} {$j <= $nflrs} {incr j} {
		set loadVal [expr $j/$sumFlrs/($nbays+1)]
		for {set i 1} {$i <= [expr $nbays+1]} {incr i} {
			set tag [expr $j*10+$i]
			load $tag $loadVal 0. 0.
		}
	}
}

set command "recorder Node -file disp2d.txt -time -node $nodeList -dof 1 2 3 disp"
eval $command
recorder display Frame 10 10 500 500 -wipe 
vup 0 1 0       
prp 0 0 1000000
display 10 5 1
# while {1} {}

numberer Plain
constraints Plain
system BandGeneral
test	NormUnbalance 1.e-6 10 0
algorithm Newton
set numSteps 1000
integrator DisplacementControl [expr $nflrs*10+1] 1 [expr 0.1*$nflrs*$H/$numSteps]

analysis Static	
analyze $numSteps
