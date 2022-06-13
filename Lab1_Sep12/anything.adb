with Ada.Text_IO; use Ada.Text_IO;
with Semaphores; use Semaphores;

procedure anything is
S:CountingSemaphore(1,1);
begin
	Ada.Text_IO.put("waiting...");
	S.Wait;
	Ada.Text_IO.put("acquires resource");
	S.Signal;
	Ada.Text_IO.put("Releases resource");
end anything;
