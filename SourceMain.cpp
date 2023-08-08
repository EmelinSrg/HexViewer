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

	//Обработка нажатых клавиш
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

	//Обработка сообщений скроллбара
	case WM_VSCROLL:
	{
		scrollInfo.fMask = SIF_ALL;
		GetScrollInfo(hScroll, SB_CTL, &scrollInfo);

		//Запоминаем позицию ползунка скроллбара до изменения позиции для дальнейшего сравнения
		scrollBuffer = scrollInfo.nPos;

		//Смещение ползунка измеряется построчно
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

		//Запоминаем позицию ползунка скроллбара после изменения его позиции
		scrollPos = scrollInfo.nPos;

		//Смещаем данные на экране
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

//Создание диалога "Open File"
void addStripMenu(HWND hWnd)
{
	hMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, 1, L"Open File");
	SetMenu(hWnd, hMenu);
}

//Создание контролов
void addControls(HWND hWnd)
{
	//HWND for static controls
	HWND hStatic;

	//Шрифт для контролов.
	HFONT hFont = CreateFont(14, 6, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Courier");

	//Создание static control, соответствующего edit control, присваивание шрифтов
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

	//Создание скроллбара
	hScroll = CreateWindowEx(WS_EX_STATICEDGE, L"Scrollbar", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, 690, 0, 20, 470, hWnd, NULL, NULL, NULL);
}

//Обработка диалога "Open File" и подготовка к началу работы
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

	//Обновляются глобальные переменные и строки для отображения в edit контролах
	wipeData();
	clearGlobalParams();

	//Проверка, готов ли файл к работе
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

//Уведомление об ошибке и отправка соответствующего сообщения системе
int displayErrorMessage()
{
	dataEditBytes = L"ERROR";
	displayData();

	return EXIT_FAILURE;
}

//Открываем файл и отображаем в память
BOOL filePrepare(LPCTSTR lpFileName)
{
	HANDLE hFile;
	if ((hFile = CreateFile(lpFileName, GENERIC_READ, NULL, NULL, OPEN_ALWAYS, NULL, NULL)) == INVALID_HANDLE_VALUE) displayErrorMessage();

	//Вычисляем размер файла
	LARGE_INTEGER fileSizeHigh;
	if ((GetFileSizeEx(hFile, &fileSizeHigh)) == INVALID_FILE_SIZE) displayErrorMessage();

	else fileSize = fileSizeHigh.QuadPart;

	//Получаем гранулярность памяти
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	dwSystemGranularity = systemInfo.dwAllocationGranularity;

	//Вычисляем количество блоков, на которые разбит файл
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

//Получаем указатель на спроецированный в память блок файла 
BOOL getDataPointer()
{
	dwBytesInBlock = dwSystemGranularity;

	//Если следующий выделяемый блок меньше гранулярности памяти, вычисляется допустимый размер блока
	if (blockBegin + dwBytesInBlock > fileSize) dwBytesInBlock = fileSize - blockBegin;

	blockEnd = blockBegin + dwBytesInBlock;

	pbFile = (PBYTE)MapViewOfFile(hMapFile, FILE_MAP_READ, blockBegin >> 32, blockBegin & 0xFFFFFFFF, dwBytesInBlock);
	if (pbFile == NULL) displayErrorMessage();

	return TRUE;
}

//Установка значений структуры скроллбара в начальное значение
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

//Добавляем новую строку для отображения
void addLine(int bytesOffset, __int64 hexOffset)
{
	CString buffer;

	//Заносим информацию о смещении в буфер, после чего передаём её в переменную для дальнейшего отображения и чистим буфер
	buffer.Format(L"%07X%i\r\n", hexOffset / 16, 0);
	dataEditOffset.Append(buffer);
	buffer.Empty();

	//Заносим информацию о байтах в шестнадцатеричном виде, чистим буфер
	for (int i = 0; i < 16; i++)
	{
		buffer.AppendFormat(L"%02X", pbFile[i + bytesOffset]);

		if (i == 15) buffer.Append(L"\r\n");
		else buffer.AppendFormat(L"%1c", ' ');
	}
	dataEditBytes.Append(buffer);
	buffer.Empty();

	//Заносим информацию о прочитанном тексте, чистим буфер
	for (int i = 0; i < 16; i++)
	{
		if (pbFile[i + bytesOffset] == '\0') buffer.AppendFormat(L"%c", '.');
		else buffer.AppendFormat(L"%c", pbFile[i + bytesOffset]);

		if (i == 15)buffer.Append(L"\r\n");
	}
	dataEditText.Append(buffer);
}

//Проверяем и смещаем текущий блок файла при необходимости, обновляем страницу для отображения
void addData(direction direction)
{
	int linesToAdd = 32;
	int blockOffset = 0;
	wipeData();

	for (int i = 0; i < linesToAdd; i++)
	{
		__int64 currentPosInBlock = static_cast<long long>(scrollPos) * line;

		//Проверка, нужно ли смещаться в другой блок при скролле вверх
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

		//Проверка, нужно ли смещаться в другой блок при скролле вниз
		if (direction == down && getCurrentBlock(currentPosInBlock + blockOffset) > currentBlock)
		{
			int blockShift = getCurrentBlock(currentPosInBlock + blockOffset);

			blockBegin = static_cast<long long>(dwBytesInBlock) * (blockShift - 1);
			currentBlock = blockShift;

			UnmapViewOfFile(pbFile);
			getDataPointer();
		}

		//Добавить строку
		addLine((currentPosInBlock + blockOffset) % dwSystemGranularity, currentPosInBlock + blockOffset);

		if (blockOffset < 496) blockOffset += line;
	}
	//Выводим обновленные данные на экран
	displayData();
}

//Вычисляем активный блок
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

//Определяем, в каком направлении нужно сместить отображаемую страницу
void shiftData()
{
	//Смещение страницы вверх
	if (scrollPos < scrollBuffer) addData(up);
	//Смещение страницы вниз
	else if (scrollPos > scrollBuffer) addData(down);
}

//Выводим данные на экран
void displayData()
{
	Edit_SetText(hEditOffset, dataEditOffset);
	Edit_SetText(hEditBytes, dataEditBytes);
	Edit_SetText(hEditText, dataEditText);
}

//Очищаем переменные для вывода на экран
void wipeData()
{
	dataEditOffset.Empty();
	dataEditBytes.Empty();
	dataEditText.Empty();
}

//Обновляем глобальные переменные
void clearGlobalParams()
{
	scrollPos = 0;
	scrollBuffer = 0;

	blockBegin = 0;
	blockEnd = 0;

	currentBlock = 1;
}
