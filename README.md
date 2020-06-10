# Validate the ns-3 implementation of TCP Veno using ns-3 DCE
--------------------

Brief about the project
------------------------
TCP Veno is also targeted to improve the performance of TCP in wireless
environments. It is a combination of Vegas and Reno, and hence, named Veno. In this
project, the aim is to validate ns-3 TCP Veno implementation by comparing the results
obtained from it to those obtained by simulating Linux TCP Veno

Table of Contents
---------------------
1.An overview
2.Build ns-3
3.Overlapping ns-3 TcpVeno with Linux veno implementation

1.An overview
----------------
ns-3 is a free open source project aiming to build a discrete-event
network simulator targeted for simulation research and education.
In brief, ns-3 provides models of how packet data networks work and perform, 
and provides a simulation engine for users to conduct simulation experiments. 
Some of the reasons to use ns-3 include to perform studies that are more difficult 
or not possible to perform with real systems.

Direct Code Execution (DCE) is a module for ns-3 that provides facilities to execute,
within ns-3, existing implementations of userspace and kernelspace network protocols or 
applications without source code changes.

The main goal of this project is to have a Linux like TCP implementation in ns-3.
In this project we have used ns-3 docker container.

2.Step To follow
--------------------
Step1: Add dumbbell-topology-ns3-receiver.cc in the dce/dce-linux-dev/source/ns-3-dce/example 
       add parse_cwnd in /ns-3-dce/utils ,and change wscript present in the dce/dce-linux-dev/source/ns-3-dce directory.

Step2: Check for the code of TcpVeno that is in the Source/ns-3-dev/src/internet/model/,
        compare this code with that of linux Tcp implementation.
        
Step3: Since changes was made in  the dev, you need to build the dev again.
       Following set of commands are used:
       
       3a)Go to “ns3dce@4c788588614e:~/dce-linux-dev$” and set environment variables :
        ------------------------------------------------------------------------------
             - export BAKE_HOME=`pwd`/bake
             - export PATH=$PATH:$BAKE_HOME
             - export PYTHONPATH=$PYTHONPATH:$BAKE_HOME
       
       3b) Configure DCE :
       -------------------
               - bake.py configure -e dce-linux-dev
       
       3c) Configure DCE :
       -------------------   
               - bake.py build -vvv
               
 3.Overlapping ns-3 TcpVeno with Linux veno implementation
 -------------------------------------------------------------
 Step 1: After certain adjustment of the code ,run the dumbbel on linux stack
        ./waf --run "dumbbelltopologyns3receiver --stack=linux -queue_disc_type=FifoQueueDisc --WindowScaling=true 
        --Sack=true -- stopTime=300 --delAckCount=1 --BQL=true" 

 Step 2:  Go to ns3-dce/utils directory and run the parse_cwnd.py within utils.
          The results can be found in results/dumbbelltopology/ there will be one folder cwnd_data where you will find
          A-linux.plotme (the linux plot).This plot is the copied to a new created directory overlapped.
          
 Step 3:  Just as linux run on the ns-3 stack
          ./waf --run "dumbbelltopologyns3receiver --stack=ns3 -queue_disc_type=FifoQueueDisc --WindowScaling=true 
          --Sack=true --stopTime=300 --delAckCount=1 --BQL=true --transport_prot=TcpNewReno"
          
 Step 4:  Go to results/dumbbelltopology/latest timestamp/cwndTraces one file of  A-ns3.plotme .
         Copy the plot to the same directory overlapped.
 
 Step 5: The twoo plots(A-linux.plotme and  A-ns3.plotme)are in the same directory and then run gnuplot 
         overlapgnuplotscriptCwnd.one file will be created CwndA.png that can be copied to home of local storage
         to view the overlapping plot.
         
               
               
