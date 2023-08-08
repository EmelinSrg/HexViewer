#include "SoftwareDefinitions.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };

	wc.hInstance = hInstance;
	wc.lpszClassName = L"HexViewerApp";
	wc.lpfnWndProc = WindowProcedure;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
		return EXIT_FAILURE;

	RECT windowRECT{ 0, 0, appWidth, appHeight };
	AdjustWindowRect(&windowRECT, WS_SYSMENU, false);

	hwnd = CreateWindow(wc.lpszClassName, L"HexViewer", WS_SYSMENU, 0, 0, windowRECT.right, windowRECT.bottom, nullptr, nullptr, wc.hInstance, nullptr);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}

//Обработка сообщений
LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{

	case WM_COMMAND:
	{
		switch (wParam)
		{
		case 1:
			openFileDialog(hWnd);
			break;
		}
	}
	return 0;

	//Обработка действий колеса мыши
	case WM_MOUSEWHEEL:
	{
		int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		if (wheelDelta > 0) SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		else if (wheelDelta < 0) SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		break;
	}

	//Îáðàáîòêà íàæàòûõ êëàâèø
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_HOME:
			SendMessage(hWnd, WM_VSCROLL, SB_TOP, NULL);
			break;
		case VK_END:
			SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, NULL);
			break;
		case VK_PRIOR:
			SendMessage(hWnd, WM_VSCROLL, SB_PAGEUP, NULL);
			break;
		case VK_NEXT:
			SendMessage(hWnd, WM_VSCROLL, SB_PAGEDOWN, NULL);
			break;
		case VK_UP:
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
			break;
		case VK_DOWN:
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
			break;
		}
	}

	//Îáðàáîòêà ñîîáùåíèé ñêðîëëáàðà
	case WM_VSCROLL:
	{
		scrollInfo.fMask = SIF_ALL;
		GetScrollInfo(hScroll, SB_CTL, &scrollInfo);

		//Çàïîìèíàåì ïîçèöèþ ïîëçóíêà ñêðîëëáàðà äî èçìåíåíèÿ ïîçèöèè äëÿ äàëüíåéøåãî ñðàâíåíèÿ
		scrollBuffer = scrollInfo.nPos;

		//Ñìåùåíèå ïîëçóíêà èçìåðÿåòñÿ ïîñòðî÷íî
		switch (LOWORD(wParam))
		{

		case SB_LINEUP:
			scrollInfo.nPos--;
			break;

		case SB_LINEDOWN:
			scrollInfo.nPos++;
			break;

		case SB_PAGEUP:
			scrollInfo.nPos -= 32;
			break;

		case SB_PAGEDOWN:
			scrollInfo.nPos += 32;
			break;

		case SB_TOP:
			scrollInfo.nPos = scrollInfo.nMin;
			break;

		case SB_BOTTOM:
			scrollInfo.nPos = scrollInfo.nMax;
			break;

		case SB_THUMBTRACK:
			scrollInfo.nPos = scrollInfo.nTrackPos;
			break;

		default:
			break;
		}

		scrollInfo.fMask = SIF_POS;
		SetScrollInfo(hScroll, SB_CTL, &scrollInfo, TRUE);
		GetScrollInfo(hScroll, SB_CTL, &scrollInfo);

		//Çàïîìèíàåì ïîçèöèþ ïîëçóíêà ñêðîëëáàðà ïîñëå èçìåíåíèÿ åãî ïîçèöèè
		scrollPos = scrollInfo.nPos;

		//Ñìåùàåì äàííûå íà ýêðàíå
		shiftData();
	}
	return 0;

	case WM_CREATE:
	{
		addStripMenu(hWnd);
		addControls(hWnd);
		break;
	}
	return 0;

	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
	}
	return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//Ñîçäàíèå äèàëîãà "Open File"
void addStripMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, 1, L"Open File");
	SetMenu(hWnd, hMenu);
}

//Ñîçäàíèå êîíòðîëîâ
void addControls(HWND hWnd)
{
	//HWND for static controls
	HWND hStatic;

	//Øðèôò äëÿ êîíòðîëîâ.
	HFONT hFont = CreateFont(14, 6, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Courier");

	//Ñîçäàíèå static control, ñîîòâåòñòâóþùåãî edit control, ïðèñâàèâàíèå øðèôòîâ
	// 1 - Offset(h)
	hStatic = CreateWindowEx(WS_EX_STATICEDGE, L"Static", L"Offset (h)", WS_VISIBLE | WS_CHILD | ES_CENTER, 0, 0, 100, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

	hEditOffset = CreateWindowEx(WS_EX_STATICEDGE, L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY, 0, 20, 100, 525, hWnd, NULL, NULL, NULL);
	SendMessage(hEditOffset, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

	// 2 - Bytes(h)
	hStatic = CreateWindowEx(WS_EX_STATICEDGE, L"Static", L"Bytes (h)", WS_VISIBLE | WS_CHILD | ES_CENTER, 100, 0, 400, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

	hEditBytes = CreateWindowEx(WS_EX_STATICEDGE, L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY | ES_LEFT, 100, 20, 400, 525, hWnd, NULL, NULL, NULL);
	SendMessage(hEditBytes, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

	// 3 - Decoded text
	hStatic = CreateWindowEx(WS_EX_STATICEDGE, L"Static", L"Decoded text", WS_VISIBLE | WS_CHILD | ES_CENTER, 500, 0, 190, 20, hWnd, NULL, NULL, NULL);
	SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

	hEditText = CreateWindowEx(WS_EX_STATICEDGE, L"Edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | ES_READONLY, 500, 20, 190, 525, hWnd, NULL, NULL, NULL);
	SendMessage(hEditText, WM_SETFONT, (WPARAM)hFont, (LPARAM)TRUE);

	//Ñîçäàíèå ñêðîëëáàðà
	hScroll = CreateWindowEx(WS_EX_STATICEDGE, L"Scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 690, 0, 20, 470, hWnd, NULL, NULL, NULL);
}

//Îáðàáîòêà äèàëîãà "Open File" è ïîäãîòîâêà ê íà÷àëó ðàáîòû
void openFileDialog(HWND hWnd)
{
	wchar_t filename[100];

	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = filename;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;

	GetOpenFileName(&ofn);

	//Îáíîâëÿþòñÿ ãëîáàëüíûå ïåðåìåííûå è ñòðîêè äëÿ îòîáðàæåíèÿ â edit êîíòðîëàõ
	wipeData();
	clearGlobalParams();

	//Ïðîâåðêà, ãîòîâ ëè ôàéë ê ðàáîòå
	if (filePrepare(ofn.lpstrFile) == TRUE && getDataPointer() == TRUE)
	{
		setScroll();
		addData(down);
	}
	else
	{
		displayErrorMessage();
	}
}

//Óâåäîìëåíèå îá îøèáêå è îòïðàâêà ñîîòâåòñòâóþùåãî ñîîáùåíèÿ ñèñòåìå
int displayErrorMessage()
{
	dataEditBytes = L"ERROR";
	displayData();

	return EXIT_FAILURE;
}

//Îòêðûâàåì ôàéë è îòîáðàæàåì â ïàìÿòü
BOOL filePrepare(LPCTSTR lpFileName)
{
	HANDLE hFile;
	if ((hFile = CreateFile(lpFileName, GENERIC_READ, NULL, NULL, OPEN_ALWAYS, NULL, NULL)) == INVALID_HANDLE_VALUE) displayErrorMessage();

	//Âû÷èñëÿåì ðàçìåð ôàéëà
	LARGE_INTEGER fileSizeHigh;
	if ((GetFileSizeEx(hFile, &fileSizeHigh)) == INVALID_FILE_SIZE) displayErrorMessage();

	else fileSize = fileSizeHigh.QuadPart;

	//Ïîëó÷àåì ãðàíóëÿðíîñòü ïàìÿòè
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	dwSystemGranularity = systemInfo.dwAllocationGranularity;

	//Âû÷èñëÿåì êîëè÷åñòâî áëîêîâ, íà êîòîðûå ðàçáèò ôàéë
	__int64 fileSizeBuffer = fileSize;
	while (fileSizeBuffer > 0)
	{
		fileSizeBuffer -= dwSystemGranularity;
		blocksInFile++;
	}

	if ((hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL)) == NULL) displayErrorMessage();
	CloseHandle(hFile);

	return TRUE;
}

//Ïîëó÷àåì óêàçàòåëü íà ñïðîåöèðîâàííûé â ïàìÿòü áëîê ôàéëà 
BOOL getDataPointer()
{
	dwBytesInBlock = dwSystemGranularity;

	//Åñëè ñëåäóþùèé âûäåëÿåìûé áëîê ìåíüøå ãðàíóëÿðíîñòè ïàìÿòè, âû÷èñëÿåòñÿ äîïóñòèìûé ðàçìåð áëîêà
	if (blockBegin + dwBytesInBlock > fileSize) dwBytesInBlock = fileSize - blockBegin;

	blockEnd = blockBegin + dwBytesInBlock;

	pbFile = (PBYTE)MapViewOfFile(hMapFile, FILE_MAP_READ, blockBegin >> 32, blockBegin & 0xFFFFFFFF, dwBytesInBlock);
	if (pbFile == NULL) displayErrorMessage();

	return TRUE;
}

//Óñòàíîâêà çíà÷åíèé ñòðóêòóðû ñêðîëëáàðà â íà÷àëüíîå çíà÷åíèå
void setScroll()
{
	scrollInfo.cbSize = sizeof(SCROLLINFO);
	scrollInfo.fMask = SIF_RANGE | SIF_PAGE;
	scrollInfo.nMin = 0;
	scrollInfo.nMax = (fileSize - 496) / line - 1;
	scrollInfo.nPos = 0;
	SetScrollInfo(hScroll, SB_CTL, &scrollInfo, TRUE);
	SetScrollPos(hScroll, SB_CTL, 0, TRUE);
}

//Äîáàâëÿåì íîâóþ ñòðîêó äëÿ îòîáðàæåíèÿ
void addLine(int bytesOffset, __int64 hexOffset)
{
	CString buffer;

	//Çàíîñèì èíôîðìàöèþ î ñìåùåíèè â áóôåð, ïîñëå ÷åãî ïåðåäà¸ì å¸ â ïåðåìåííóþ äëÿ äàëüíåéøåãî îòîáðàæåíèÿ è ÷èñòèì áóôåð
	buffer.Format(L"%07X%i\r\n", hexOffset / 16, 0);
	dataEditOffset.Append(buffer);
	buffer.Empty();

	//Çàíîñèì èíôîðìàöèþ î áàéòàõ â øåñòíàäöàòåðè÷íîì âèäå, ÷èñòèì áóôåð
	for (int i = 0; i < 16; i++)
	{
		buffer.AppendFormat(L"%02X", pbFile[i + bytesOffset]);

		if (i == 15) buffer.Append(L"\r\n");
		else buffer.AppendFormat(L"%1c", ' ');
	}
	dataEditBytes.Append(buffer);
	buffer.Empty();

	//Çàíîñèì èíôîðìàöèþ î ïðî÷èòàííîì òåêñòå, ÷èñòèì áóôåð
	for (int i = 0; i < 16; i++)
	{
		if (pbFile[i + bytesOffset] == '\0') buffer.AppendFormat(L"%c", '.');
		else buffer.AppendFormat(L"%c", pbFile[i + bytesOffset]);

		if (i == 15)buffer.Append(L"\r\n");
	}
	dataEditText.Append(buffer);
}

//Ïðîâåðÿåì è ñìåùàåì òåêóùèé áëîê ôàéëà ïðè íåîáõîäèìîñòè, îáíîâëÿåì ñòðàíèöó äëÿ îòîáðàæåíèÿ
void addData(direction direction)
{
	int linesToAdd = 32;
	int blockOffset = 0;
	wipeData();

	for (int i = 0; i < linesToAdd; i++)
	{
		__int64 currentPosInBlock = static_cast<long long>(scrollPos) * line;

		//Ïðîâåðêà, íóæíî ëè ñìåùàòüñÿ â äðóãîé áëîê ïðè ñêðîëëå ââåðõ
		if (direction == up && getCurrentBlock(currentPosInBlock + blockOffset) < currentBlock)
		{
			int blockShift = getCurrentBlock(currentPosInBlock + blockOffset);
			while (blockShift < currentBlock)
			{
				if (blockBegin - dwSystemGranularity >= 0) blockBegin -= dwSystemGranularity;

				UnmapViewOfFile(pbFile);
				getDataPointer();




				currentBlock--;
			}
		}

		//Ïðîâåðêà, íóæíî ëè ñìåùàòüñÿ â äðóãîé áëîê ïðè ñêðîëëå âíèç
		if (direction == down && getCurrentBlock(currentPosInBlock + blockOffset) > currentBlock)
		{
			int blockShift = getCurrentBlock(currentPosInBlock + blockOffset);

			blockBegin = static_cast<long long>(dwBytesInBlock) * (blockShift - 1);
			currentBlock = blockShift;

			UnmapViewOfFile(pbFile);
			getDataPointer();
		}

		//Äîáàâèòü ñòðîêó
		addLine((currentPosInBlock + blockOffset) % dwSystemGranularity, currentPosInBlock + blockOffset);

		if (blockOffset < 496) blockOffset += line;
	}
	//Âûâîäèì îáíîâëåííûå äàííûå íà ýêðàí
	displayData();
}

//Âû÷èñëÿåì àêòèâíûé áëîê
int getCurrentBlock(__int64 currentScrollPos)
{
	int block = 0;
	while (currentScrollPos >= 0)
	{
		currentScrollPos -= dwSystemGranularity;
		block++;
	}
	return block;
}

//Îïðåäåëÿåì, â êàêîì íàïðàâëåíèè íóæíî ñìåñòèòü îòîáðàæàåìóþ ñòðàíèöó
void shiftData()
{
	//Ñìåùåíèå ñòðàíèöû ââåðõ
	if (scrollPos < scrollBuffer) addData(up);
	//Ñìåùåíèå ñòðàíèöû âíèç
	else if (scrollPos > scrollBuffer) addData(down);
}

//Âûâîäèì äàííûå íà ýêðàí
void displayData()
{
	Edit_SetText(hEditOffset, dataEditOffset);
	Edit_SetText(hEditBytes, dataEditBytes);
	Edit_SetText(hEditText, dataEditText);
}

//Î÷èùàåì ïåðåìåííûå äëÿ âûâîäà íà ýêðàí
void wipeData()
{
	dataEditOffset.Empty();
	dataEditBytes.Empty();
	dataEditText.Empty();
}

//Îáíîâëÿåì ãëîáàëüíûå ïåðåìåííûå
void clearGlobalParams()
{
	scrollPos = 0;
	scrollBuffer = 0;

	blockBegin = 0;
	blockEnd = 0;

	currentBlock = 1;
}
