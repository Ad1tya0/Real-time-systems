-- Package: Semaphores
--
-- ==> Complete the code at the indicated places

package body Semaphores is
   protected body CountingSemaphore is
      entry Wait when Count > 0 is --If There is space in the buffer(Count>0) we pass a semaphore and decrement the semaphore count.
      	begin
      	Count:=Count-1;
      end Wait;

      entry Signal when Count < MaxCount is --Release but never give more than resource size
      	begin
      	Count:=Count+1;
      end Signal;

   end CountingSemaphore;
end Semaphores;

