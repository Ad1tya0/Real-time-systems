with Ada.Text_IO;
use Ada.Text_IO;

package body Buffer is
   protected body CircularBuffer is

      entry Put(X: Item) when Count < Size is
      
      begin
         A(In_Ptr) := X;
         In_Ptr := In_Ptr + 1;
         Count := Count + 1;
         written:=written+1;
         buffervalue:=buffervalue+1;
         Put_Line("writing to buffer, written=");
         Put_Line(Integer'Image(written));
         Put_Line("Buffer value is=");
         Put_Line(Integer'Image(Count));
      end Put;

      entry Get(X: out Item) when Count > 0 is
      begin
         X := A(Out_Ptr);
         Out_Ptr := Out_Ptr + 1;
         Count := Count - 1;
         read:=read+1;
         buffervalue:=buffervalue-1;
         Put_Line("Reading from buffer, read=");
         Put_Line(Integer'Image(read));
         Put_Line("Buffer value is=");
         Put_Line(Integer'Image(Count));
      end Get;
   end CircularBuffer;
end Buffer;
