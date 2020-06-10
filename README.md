#  Validate the ns-3 implementation of TCP Veno using ns-3 DCE
--------------------

Brief about the project
------------------------
TCP Veno is  targeted to improve the performance of TCP in wireless
environments. It is a combination of Vegas and Reno, and hence, named Veno. In this
project, the aim is to validate ns-3 TCP Veno implementation by comparing the results
obtained from it to those obtained by simulating Linux TCP Veno

Table of Contents
---------------------
1. An overview
2. Build ns-3
3. Overlapping ns-3 TcpVeno with Linux veno implementation

1.An overview
----------------
ns-3 is a free open source project aiming to build a discrete-event
network simulator targeted for simulation research and education.
In brief, ns-3 provides models of how packet data networks work and perform, 
and provides a simulation engine for users to conduct simulation experiments. 
Some of the reasons to use ns-3 include to perform studies that are more difficult 
or not possible to perform with real systems.
[For more information click on this Link](http://www.nsnam.org)

Direct Code Execution (DCE) is a module for ns-3 that provides facilities to execute,
within ns-3, existing implementations of userspace and kernelspace network protocols or 
applications without source code changes.

The main goal of this project is to have a Linux like TCP implementation in ns-3.
In this project we have used ns-3 docker container.

2.Steps To Follow
--------------------
Step1. Add dumbbell-topology-ns3-receiver.cc in the dce/dce-linux-dev/source/ns-3-dce/example 
       add parse_cwnd in /ns-3-dce/utils ,and change wscript present in the dce/dce-linux-dev/source/ns-3-dce directory.

Step2. Check for the code of TcpVeno that is in the Source/ns-3-dev/src/internet/model/,
        compare this code with that of linux Tcp implementation.
        
Step3. Since changes was made in  the dev, you need to build the dev again.
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
Step 1. After making necessary changes(tcp-veno.cc,tcp-congestion-ops.cc,tcp-congestion-ops.h,tcp-veno.h)
          copy the files to source/ns-3-dev/src/internet/model.
          Run the dumbbelltopology in linux stack of ns-3-dce in the directory source/ns-3-dce/ using
         ./waf --run ”dumbbelltopologyns3receiver --stack=linux --queue_disc_type=FifoQueueDisc --WindowScaling=true
          -- Sack=true --stopTime=300 --delAckCount=1 --BQL=true --linux_prot=veno”. 

 Step 2. Copy the parse_cwnd.py file in ns-3-dce/utils/ using sudo docker cp parse_cwnd.py your docker     name:/home/ns3dce/dce-linux-dev/source/ns-3-dce/utilsand run there using python parse_cwnd.py 2 2.
 Running this file will collect the traces generated by running the topology example on linux stack. This will create a   folder cwnd_data inside ns-3-dce/result/dumbbell-topology where there will be A-linux.plotme file.
 
 Step 3.   Make one folder ”overlapped” inside ns-3-dce/result/dumbell-topology/ and copy the 
            A-linux.plotme file from cwnd data to overlapped folder.
 
 Step 4. Run the dumbbelltopology in ns3 stack using the command
          ./waf --run "dumbbelltopologyns3receiver --stack=ns3 -queue_disc_type=FifoQueueDisc --WindowScaling=true 
          --Sack=true --stopTime=300 --delAckCount=1 --BQL=true --transport_prot=TcpVeno --recovery=TcpPrrRecovery"
           This will create a new timestamp folder under ns-3-dce/result/dumbbell-topology/ and under 
           the latest timestamp folder, find a folder ”cwndTraces” where A-ns3.plotme file will be generated.
           Copy this file in overlapped folder.
          
 Step 5.  Now copy the overlap-gnuplotscriptCwnd script inside overlapped using sudo docker 
          cp overlap-gnuplotscriptCwnd your  docker name:/home/ns3dce/dce-linux-dev/source/ns-3-dce 
          /results/dumbbell-topology/overlappedand install gnuplot in directory overlapped using 
           sudo apt-get install gnuplot.Then run gnuplot overlap-gnuplotscriptCwnd.
 
 Step 6. Above steps will create a CwndA.png file inside overlapped which we can copy to home directory
        using sudo docker cp dockername:/home/ns3dce/dce-linux-dev/source/ns-3-dce/results/dumbbell-topology/overlapped/CwndA.png .
         
               
               
