with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Ada.Numerics.Discrete_Random;

procedure ProducerConsumer_Rndzvs is
	
   N : constant Integer := 10; -- Number of produced and consumed tokens per task
	X : constant Integer := 3; -- Number of producers and consumers	
	
   -- Random Delays
   subtype Delay_Interval is Integer range 50..700;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
   use Random_Delay;
   G : Generator;

   task Buffer is
      entry Append(I : in Integer);
      entry Take(I : out Integer);
   end Buffer;

   task type Producer;

   task type Consumer;
   
   task body Buffer is
         Size: constant Integer := 4;
         type Index is mod Size;
         type Item_Array is array(Index) of Integer;
         B : Item_Array;
         In_Ptr, Out_Ptr, Count : Index := 0;
         read,written,buffervalue: Integer:=0; --debug variables
   begin
      loop
         select
				-- => Complete Code: Service Append
            when buffervalue<Size =>
               accept Append(I: in Integer) do
                  B(In_Ptr):=I;
                  In_Ptr:=In_Ptr+1;
                  Count:=Count+1;
                  buffervalue:=buffervalue+1;
                  written:=written+1;
                  Put_Line("writing to buffer, written=");
                  Put_Line(Integer'Image(written));
                  Put_Line("Buffer value is=");
                  Put_Line(Integer'Image(buffervalue));
               end Append;
         or
				-- => Complete Code: Service Take
            when buffervalue>0 =>
               accept Take(I: out Integer) do
                  I:=B(Out_Ptr);
                  Out_Ptr:=Out_Ptr+1;
                  Count:=Count-1;
                  buffervalue:=buffervalue-1;
                  read:=read+1;
                  Put_Line("reading from buffer, read=");
                  Put_Line(Integer'Image(read));
                  Put_Line("Buffer value is=");
                  Put_Line(Integer'Image(buffervalue));
               end Take;
         or
				-- => Termination
            terminate;
         end select;
      end loop;
   end Buffer;
      
   task body Producer is
      Next : Time;
      typcast: Integer;
   begin
      Next := Clock;
      for I in 1..N loop
      -- => Write to X

			typcast:=I;
         Buffer.Append(typcast);

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
         Buffer.Take(X);
         Put_Line("data retrieved from buffer is=");
         Put_Line(Integer'Image(X));
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;
	
	P: array (Integer range 1..X) of Producer;
	C: array (Integer range 1..X) of Consumer;
	
begin -- main task
   null;
end ProducerConsumer_Rndzvs;


