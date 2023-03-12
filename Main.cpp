//
//	The MIT License
//
//	Copyright (c) 2010 James E Beveridge
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.


//	This sample code is for my blog entry titled, "Understanding ReadDirectoryChangesW"
//	http://qualapps.blogspot.com/2010/05/understanding-readdirectorychangesw.html
//	See ReadMe.txt for overview information.


#include "stdafx.h"
#include "ReadDirectoryChanges.h"


LPCWSTR ExplainAction( DWORD dwAction );
bool TryGetKeyboardInput( HANDLE hStdIn, bool &bTerminate, std::wstring& buf);


//
// When the application starts, it immediately starts monitoring your home
// directory, including children, as well as C:\, not including children.
// The application exits when you hit Esc.
// You can add a directory to the monitoring list by typing the directory
// name and hitting Enter. Notifications will pause while you type.
//

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	constexpr DWORD dwNotificationFlags =
		FILE_NOTIFY_CHANGE_LAST_WRITE
		| FILE_NOTIFY_CHANGE_CREATION
		| FILE_NOTIFY_CHANGE_FILE_NAME;

	// Create the monitor and add two directories.
	CReadDirectoryChanges changes;
	changes.AddDirectory(_tgetenv(_T("USERPROFILE")), true, dwNotificationFlags);
	changes.AddDirectory(_T("C:\\"), false, dwNotificationFlags);

	HANDLE hStdIn =  ::GetStdHandle(STD_INPUT_HANDLE);
	const HANDLE handles[] = { hStdIn, changes.GetWaitHandle() };

	std::wstring buf;
	bool bTerminate = false;

	while (!bTerminate)
	{
		DWORD rc = ::WaitForMultipleObjectsEx(_countof(handles), handles, false, INFINITE, true);
		switch (rc)
		{
		case WAIT_OBJECT_0 + 0:
			// hStdIn was signaled. This can happen due to mouse input, focus change,
			// Shift keys, and more.  Delegate to TryGetKeyboardInput().
			// TryGetKeyboardInput sets bTerminate to true if the user hits Esc.
			if (TryGetKeyboardInput(hStdIn, bTerminate, buf))
				changes.AddDirectory(buf.c_str(), false, dwNotificationFlags);
			break;
		case WAIT_OBJECT_0 + 1:
			// We've received a notification in the queue.
			{
				DWORD dwAction;
				std::wstring wstrFilename;
				if (changes.CheckOverflow())
					wprintf(L"Queue overflowed.\n");
				else
				{
					changes.Pop(dwAction, wstrFilename);
					wprintf(L"%s %s\n", ExplainAction(dwAction), wstrFilename.c_str());
				}
			}
			break;
		case WAIT_IO_COMPLETION:
			// Nothing to do.
			break;
		}
	}

	// Just for sample purposes. The destructor will
	// call Terminate() automatically.
	changes.Terminate();

	return EXIT_SUCCESS;
}

LPCWSTR ExplainAction( DWORD dwAction )
{
	switch (dwAction)
	{
	case FILE_ACTION_ADDED            :
		return L"Added";
	case FILE_ACTION_REMOVED          :
		return L"Deleted";
	case FILE_ACTION_MODIFIED         :
		return L"Modified";
	case FILE_ACTION_RENAMED_OLD_NAME :
		return L"Renamed From";
	case FILE_ACTION_RENAMED_NEW_NAME :
		return L"Renamed To";
	default:
		return L"BAD DATA";
	}
}

bool TryGetKeyboardInput( HANDLE hStdIn, bool &bTerminate, std::wstring& buf )
{
	DWORD dwNumberOfEventsRead=0;
	INPUT_RECORD rec{};

	if (!::PeekConsoleInput(hStdIn, &rec, 1, &dwNumberOfEventsRead))
		return false;

	if (rec.EventType == KEY_EVENT)
	{
		if (rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
			bTerminate = true;
		else if (rec.Event.KeyEvent.wVirtualKeyCode > VK_HELP)
		{
			WCHAR buf[MAX_PATH];
			if (!_getws_s(buf, _countof(buf)))	// End of file, usually Ctrl-Z
				bTerminate = true;
			else
				return true;
		}
	}

	::FlushConsoleInputBuffer(hStdIn);

	return false;
}
