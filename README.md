# Rickware

The idea is to destroy the whole os using Rick Astley, but now it only hooks
the ShellExecuteA function to always open Never Gonna Give You Up.

### Build

Use the provided cmake and after compilation you should have one dll file and
two exe files (only Windows is supported by the way).

### Rickware.exe

Performs dll injection on a process that you specify using its PID (you can 
look it up using the tasklist command).

### Victim.exe

An executable that you can safely test Rickware on. All it does is display a
link to example.com that you can corrupt.

### Notes

- RickExecute.dll must be in the same directory as Rickware.exe.
- The injection is in its infancy right now, so you will probably need to 
disable your antivirus for it to work.