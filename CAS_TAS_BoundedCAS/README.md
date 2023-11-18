# CAS-TAS-Bounded_CAS
The base idea of this program is to implement TAS,CAS and Bounded CAS and calculating average and worst case waiting time for each of these algorithms  
1) Browse to the directory where all the files are present.There must be a "input-param.txt" file with n,k,lambda1, lambda2 values
   if compliling the files individually.

2) To Compile each of the file one by one and execute them all  
    a. TAS          :   $ g++ TAS.cpp -lpthread -o tas (for compling) and ./tas for executing  
    b. CAS          :   $ g++ CAS.cpp -lpthread -o cas(for compling) and ./cas for executing  
    c. CAS-Bounded  :   $ g++ CAS-Bounded.cpp -lpthread -o cas-bound(for compling) and ./cas-bound for executing  


3) The log is written to a file named (filename)-log.txt and stats are written to a file called file named (filename)-Stats.tx

The filenames can be:  
a1 tas  
a2 cas-bounded  
a3 cas  
