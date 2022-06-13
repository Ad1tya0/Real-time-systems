package body Buffer is
   protected body CircularBuffer is

      entry Put(X: in Item) when Count < Size is
      begin
         A(In_Ptr) := X; --A is an array while In_Ptr points to the next empty position
         In_Ptr := In_Ptr + 1; -- Object is added to the array and In_Ptr is incremented
         Count := Count + 1; 
      end Put;

      entry Get(X: out Item) when Count > 0 is
      begin
         X := A(Out_Ptr);
         Out_Ptr := Out_Ptr + 1;
         Count := Count - 1;
      end Get;
   end CircularBuffer;
end Buffer;
