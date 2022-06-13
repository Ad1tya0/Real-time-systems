with Ada.Text_IO;                       use Ada.Text_IO;
with Ada.Integer_Text_IO;           use Ada.Integer_Text_IO;
package body Buffer is
   protected body CircularBuffer is

      entry Put(X: in Item) when Count < Size is --Buffer not Full
      begin
         A(In_Ptr) := X; --A is an array while In_Ptr points to the next empty position
         In_Ptr := In_Ptr + 1; -- Object is added to the array and In_Ptr is incremented
         Count := Count + 1;
         written := written+1;
         Put_Line("Writing, written =");
         Put_Line(Integer'Image(written));
         Put_Line("BufferCount= ");
         Put_Line(Integer'Image(Count));

      end Put;

      entry Get(X: out Item) when Count > 0 is --Buffer not Empty
      begin
         X := A(Out_Ptr);
         Out_Ptr := Out_Ptr + 1;
         Count := Count - 1;
         read := read+1;
         Put_Line("Reading, read =");
         Put_Line(Integer'Image(read));
         Put_Line("BufferCount= ");
         Put_Line(Integer'Image(Count));

      end Get;
   end CircularBuffer;
end Buffer;
