with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

procedure ProducerConsumer_Rndzvs is
	
   N : constant Integer := 10; -- Number of produced and consumed tokens per task
   X : constant Integer := 3; -- Number of producers and consumers
   written, read: Integer :=0;	
	
   -- Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
   use Random_Delay;
   G : Generator;

   task Buffer is
      entry Append(I : in Integer); --Producer task will call this function to append to the buffer
      entry Take(I : out Integer); --Consumer task will call this function to take from the buffer
      --The accept body needs to be implemented with a guard to fulfill the access rules
   end Buffer;

   task type Producer;

   task type Consumer;
   
   task body Buffer is
         Size: constant Integer := 4;
         type Index is mod Size;
         type Item_Array is array(Index) of Integer;
         B : Item_Array;
         myCount: Integer:= 0;
         In_Ptr, Out_Ptr, Count : Index := 0;
   begin
      loop
         select
				-- => Complete Code: Service Append
				-- The accept for the Append entry is implemented here.
				when myCount < Size => --Making Sure Buffer isn't Full
					accept Append(I : in Integer) do
						B(In_Ptr):= I;
						In_Ptr := In_Ptr+1;
						myCount:= myCount+1;
						written:=written+1;
						Put_Line("writing, written =");
				        Put_Line(Integer'Image(written));
				        Put_Line("BufferCount= ");
				        Put_Line(Integer'Image(myCount));

						end Append;
         or
				-- => Complete Code: Service Take
				-- The accept for the Take entry is implemented here.
				when myCount > 0 => --Making Sure Buffer isn't Empty
					accept Take(I : out Integer) do
						I:=B(Out_Ptr);
						Out_Ptr := Out_Ptr+1;
						myCount:= myCount-1;
						read:=read+1;
						Put_Line("reading, read =");
				        Put_Line(Integer'Image(read));
				        Put_Line("BufferCount= ");
				        Put_Line(Integer'Image(myCount));
						end Take;
         or
				-- => Termination
				-- Call the terminate function here, this will only execute when all other tasks have finished
				terminate;
         end select;
      end loop;
   end Buffer;
      
   task body Producer is
      Next : Time;
      Z : Integer:=0;
   begin
      Next := Clock;
      for I in 1..N loop
			
         -- => Complete code: Write to X
         -- After implementing the function bodies above, you need to call them here.
         Z:= I;
         Buffer.Append(Z);
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
         -- Complete Code: Read from X
         -- After implementing the function bodies above, you need to call them here.
         Buffer.Take(X);
         --Put_Line(Integer'Image(X)); --Printing out what's read.
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;
	
	P: array (Integer range 1..X) of Producer;
	C: array (Integer range 1..X) of Consumer;
	
begin -- main task
   null;
end ProducerConsumer_Rndzvs;


