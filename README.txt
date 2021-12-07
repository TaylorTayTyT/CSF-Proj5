Taylor Nguyen/Cesco Ferrara: DID EVERYTHING TOGETHER :)

To make our data safe to access from multiple threads, we locked and unlocked critical sections of data. We used the mutex object to do this. We determined that our calc_eval function,
which does all calculations and assignments, was our critical section because it accesses and updates shared variables. Hence, we lock before we do this section and then unlock
once it is completed. 
