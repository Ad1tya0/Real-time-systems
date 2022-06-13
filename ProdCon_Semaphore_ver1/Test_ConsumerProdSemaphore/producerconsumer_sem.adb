with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

with Semaphores;
use Semaphores;

procedure ProducerConsumer_Sem is
	
	N : constant Integer := 10; -- Number of produced and consumed tokens per task
	X : constant Integer := 3; -- Number of producers and consumer
		
	-- Buffer Definition
	Size: constant Integer := 4;
	type Index is mod Size;
	type Item_Array is array(Index) of Integer;
	B : Item_Array;
   read, written, bufferCount : Integer:= 0;
	In_Ptr, Out_Ptr, Count : Index := 0;

   -- Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
   use Random_Delay;
   G : Generator;
	
   -- => Complete code: Declation of Semaphores
	--    1. Semaphore 'NotFull' to indicate that buffer is not full, deals with producers
   NotFull: CountingSemaphore(4,4);
	--    2. Semaphore 'NotEmpty' to indicate that buffer is not empty, this deals with consumers
   NotEmpty: CountingSemaphore(4,0);
	--    3. Semaphore 'AtomicAccess' to ensure an atomic access to the buffer
   AtomicAccess: CountingSemaphore(1,1);
	
   task type Producer;

   task type Consumer;

   task body Producer is
      Next : Time;
      prodCount: Integer:= 1;
   begin
      Next := Clock;
      for I in 1..N loop
         -- => Complete Code: Write to Buffer
         prodCount:=I;
			NotFull.Wait; --check that buffer isn't full
         AtomicAccess.Wait;
         B(In_Ptr):=I;
         In_Ptr := In_Ptr + 1; -- Object is added to the array and In_Ptr is incremented
         bufferCount := bufferCount + 1;
         written := written+1;
         Put_Line("Writing, written =");
         Put_Line(Integer'Image(written));
         Put_Line("BufferCount= ");
         Put_Line(Integer'Image(bufferCount));
         NotEmpty.Signal;
         AtomicAccess.Signal;
         -- Next 'Release' in 50..250ms
         Next := Next + Milliseconds(Random(G));
         delay until Next;
         
      end loop;
   end;

   task body Consumer is
      Next : Time;
      X : Integer;
   begin
      Next := Clock;
      
      for I in 1..N loop
         -- => Complete Code: Read from Buffer
			NotEmpty.Wait;
         AtomicAccess.Wait;
         X:=B(Out_Ptr);
         Out_Ptr := Out_Ptr + 1;
         bufferCount := bufferCount - 1;
         read := read+1;
         Put_Line("reading, read =");
         Put_Line(Integer'Image(read));
         Put_Line("BufferCount= ");
         Put_Line(Integer'Image(bufferCount));
         NotFull.Signal;
         AtomicAccess.Signal;
			-- Next 'Release' in 50..250ms
         Next := Next + Milliseconds(Random(G));
         delay until Next;
         

      end loop;
   end;
	
	P: array (Integer range 1..X) of Producer;
	C: array (Integer range 1..X) of Consumer;
	
begin -- main task
   null;
end ProducerConsumer_Sem;


