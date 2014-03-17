Name:

How long did the lab take?
10 Hours

Does your lab work as expected?
Yes it does

Are there any issues?
No

What design choices did you make and why?
Decided to use an array instead of the provided linked list. It was a lot easier to 
implement and use an array. I first sort the jobs by arrival time to simulate jobs arriving in order.
I also have a job queue where jobs get stored once they arrive at the appropriate tick time. This
queue is then sorted depending on the scheduler. I change which part of the list gets sorted by using a
pointer and this is a job is "popped out" of the queue. 
