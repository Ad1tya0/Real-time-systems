package Buffer is
   Size: constant Integer := 3;
   subtype Item is Integer;
   type Index is mod Size; --This means that the index starts from 0 to Size-1
   --Declaring an array --> type <array_name> is array(Size) of <type of array elements>
   type Item_Array is array(Index) of Item; --Item is the type of elements in the array called Item_Array

   protected type CircularBuffer is
      entry Put(X: in Item);
      entry Get(X: out Item);
   private
      A: Item_Array;
      In_Ptr, Out_Ptr: Index := 0;
      Count: Integer range 0..Size := 0; --This indicates the number of elements in the buffer
      read, written : Integer :=0; --This is here just to track total read and written
   end CircularBuffer;
end Buffer;

