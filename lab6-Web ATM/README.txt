實作了三個版本
1. lab6_server_shm.c   : process + share memory
2. lab6_server_thread.c : threads 
3. lab6_server.c  在這題不用create thread/process 也可做，因這題client連進來就只做一件事(存or提錢)，所以有沒有用平行化的方式影響不大，順序做也沒差，且因程式執行速度快，有沒有用也看不出來