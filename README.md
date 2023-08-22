# festival_schedule_generator_challenge
Festival Schedule Generator Challange solution in cpp.

Two solutions are presented


# # Solution 1 with priority queue (preferred) 

This solution requires a sorted input.
While browsing through the vector of shows one can create a priority queue, which size corresponds to the number of the currently planned stages at that moment.  
This queue contains the end time of the last show planned for each stage, together with the stage number.
The ordering is such that the stage hosting the show with the earliest end time is on top of the queue (when same end time, the stage number is leading, the smaller is on top).  

The idea is that if input is sorted, one can go though the list of shows and check the "current" show end time and compare it with the top element of the queue.
If the "current" show end time is bigger than the smallest (minimum) end time of the last shows already planned for any of the already existing stages (aka the top of the queue), than the current show can fit the stage on top of the queue.

In this scenario, the queue is popped of the top value and it is added with the current show information (as it is the new last show of its stage).
Otherwise the current show cannot fit any stages and a new stage needs to be created. The queue is added with the information about the new stage (as the current show is the new last show of its stage).
 
 Time complexity: ~O(nlog(n)) for sorting and ~O(nlog(n)) for the algorithm using the priority queue.


 # # Solution 2 with nested loops (not preferred)

This Solution does not necessarily require a sorted input, however in order to present a proper (sorted) planning as output, sorting needs to happen at some point (can before execution as  presented in this solutin or after it on the result -the planning- itself).

Approach is simpler, each show is compared with the shows already planned for each stage. As soon as the show can fit a stage (meaning it does not overlap with the other shows in that stage), it is planned for that stage and the check moves to the next show in the list.  
If the current show cannot fit any stage, then a new stage is created and the show is planned for that stage.
  
Time complexity: ~O(nlog(n)) for sorting and ~O(n*n) for the algorithm using nested loops

# # Input format

Input must be preseted as a txt file named "input.txt" where each line contains the following information about one show:
* show name
* show start time (as offset from start time of the festival, in hours)
* show end time (as offset from start time of the festival, in hours)

"show name", "show start time" and "show end time" are separated by one blank space character (" ").
