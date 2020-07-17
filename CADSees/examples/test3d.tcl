
#############################model to be used for testing CADSees program########################
######################developed by Seyed Alireza Jalali (seyyed-jalali@aut.ac.ir)################
######################at Amirkabir University of Technology (OpenSees.aut.ac.ir)#################


logCommands -file testCommands3d 			;#We cannot have AutoCAD understand TCL commands while importing our model
											;#all we currently need to draw deformed model geometry in CAD will be
											;#generated using this command in a file named "testCommands.tcl"
											;#if OpenSees blames that "logCommands" is not recognized as a command,
											;#that is because you are not using the AUT version of OpenSees.exe (download from OpenSees.aut.ac.ir)
set nflrs 6
set nbaysX 3
set nbaysY 4
set Lx 600.			;#bay length in cm
set Ly 600.			;#bay length in cm
set H 300.			;#story height in cm
#W10X12 properties in cm
set Iz 2252.5
set Area 22.9

#assuming the sections to be symmetric around local y and z axes:
set Iy $Iz
set J [expr ($Iy**2.+$Iz**2.)**0.5]
#steel material
set E 2.1e6
set Nu 0.3
set G [expr $E/(1.+2.*$Nu)]

model basic -ndm 3 -ndf 6
set nodeList [list]							;#we will need this if we are combining all node disps in a single recorder file
for {set j 0} {$j <= $nflrs} {incr j} {
	for {set k 1} {$k <= [expr $nbaysY+1]} {incr k} {
		for {set i 1} {$i <= [expr $nbaysX+1]} {incr i} {
			set tag [expr $j*100+$k*10+$i]
			node $tag [expr ($i-1)*$Lx] [expr ($k-1)*$Ly] [expr $j*$H]
			if {$j != 0} {
				lappend nodeList $tag
			}
		}
	}
}
fixZ 0. 1 1 1 1 1 1
#columns
geomTransf Linear 1 1. 0. 1.
set transfTag 1
for {set j 1} {$j <= $nflrs} {incr j} {
	for {set k 1} {$k <= [expr $nbaysY+1]} {incr k} {
		for {set i 1} {$i <= [expr $nbaysX+1]} {incr i} {
			set tag [expr 1000+$j*100+$k*10+$i]
			set iNode [expr ($j-1)*100+$k*10+$i]
			set jNode [expr $j*100+$k*10+$i]
			element elasticBeamColumn $tag $iNode $jNode $Area $E $G $J $Iy $Iz $transfTag
		}
	}
}

#beams
geomTransf Linear 2 1. 1. 0.
set transfTag 2

#x-dir beams
for {set j 1} {$j <= $nflrs} {incr j} {
	for {set k 1} {$k <= [expr $nbaysY+1]} {incr k} {
		for {set i 1} {$i <= $nbaysX} {incr i} {
			set tag [expr 2000+$j*100+$k*10+$i]
			set iNode [expr $j*100+$k*10+$i]
			set jNode [expr $j*100+$k*10+$i+1]
			element elasticBeamColumn $tag $iNode $jNode $Area $E $G $J $Iy $Iz $transfTag
		}
	}
}

#y-dir beams

for {set j 1} {$j <= $nflrs} {incr j} {
	for {set i 1} {$i <= [expr $nbaysX+1]} {incr i} {
		for {set k 1} {$k <= $nbaysY} {incr k} {
			set tag [expr 3000+$j*100+$k*10+$i]
			set iNode [expr $j*100+$k*10+$i]
			set jNode [expr $j*100+($k+1)*10+$i]
			element elasticBeamColumn $tag $iNode $jNode $Area $E $G $J $Iy $Iz $transfTag
		}
	}
}

set sumFlrs [expr $nflrs*($nflrs+1)*0.5]
pattern Plain 1 Linear {
	for {set j 1} {$j <= $nflrs} {incr j} {
		set loadVal [expr $j/$sumFlrs/($nbaysX+1)/($nbaysY+1)]
		for {set k 1} {$k <= [expr $nbaysY]} {incr k} {
			for {set i 1} {$i <= [expr $nbaysX+1]} {incr i} {
				set tag [expr $j*100+$k*10+$i]
				load $tag $loadVal 0. 0. 0. 0. 0.
			}
		}
	}
}

set command "recorder Node -file disp3d.txt -time -node $nodeList -dof 1 2 3 4 5 6 disp"
eval $command
# recorder display Frame 10 10 500 500 -wipe 
# vup 1 1 1       
# prp 0 0 1000000
# display 10 5 1
# while {1} {}

numberer Plain
constraints Plain
system BandGeneral
test	NormUnbalance 1.e-6 10 0
algorithm Newton
set numSteps 100
integrator DisplacementControl [expr $nflrs*100+1*10+1] 1 [expr 0.1*$nflrs*$H/$numSteps]

analysis Static	
analyze $numSteps
