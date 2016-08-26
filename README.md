# ReadDirectoryChanges

Status:		Release candidate. This code has been tested, but not exhaustively.

This sample code goes with my blog entry titled, [Understanding ReadDirectoryChangesW](
http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html)

Please create a new issue or pull request for issues with this code.

This project is for Visual Studio 2010. It should build and work fine under
Visual Studio 2005 through 2015, although you'll need new solution and
project files for older versions. All project options were left at the default.

## Usage

When the application starts, it immediately starts monitoring your home
directory, including children, as well as C:\, not including children.
The application exits when you hit Esc.

You can add a directory to the monitoring list by typing the directory
name and hitting Enter. Notifications will pause while you type.

## Code overview

This sample includes four classes and a `main()` function to drive them.

CReadDirectoryChanges

This is the public interface.  It starts the worker thread and shuts it down,
provides a waitable queue, and allows you to add directories.

CReadChangesServer

This is the server class that runs in the worker thread.  One instance of this
object is allocated for each instance of CReadDirectoryChanges.  This class is
responsible for thread startup, orderly thread shutdown, and shimming the
various C-style callback functions to C++ member functions.

CReadChangesRequest

One instance of CReadChangesRequest is created for each monitored directory.
It contains the OVERLAPPED structure, the data buffer, the completion
notification function, and code to unpack the notification buffer.  When
a notification arrives, the data is unpacked and each change is sent to
the queue in CReadDirectoryChanges. All instances of this class run in
the worker thread

CThreadSafeQueue

Generic template class that provides a thread-safe, bounded queue that
can be waited on using any of the Win32 WaitXxx functions.


## Implementation notes

This is a Unicode project. I haven't tried to build it single-byte, but
it should work fine.

The only notably missing function is RemoveDirectory.  This is straightforward
to implement.  Create the appropriate shim and member functions in
CReadChangesServer. In the member function, search `m_pBlocks` for the directory,
call `RequestTermination()` on that block, and delete the block from the `m_pBlocks` vector.
