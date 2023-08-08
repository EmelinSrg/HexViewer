#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <iomanip>
#include <atlstr.h>
#include <winbase.h>

/*Перечисления*/
enum dataBlockInBytes
{
	page = 512,
	line = 16
};

enum direction
{
	up,
	down
};

//Прототипы функций
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void addStripMenu(HWND hWnd);
void addControls(HWND hWnd);
void openFileDialog(HWND hWnd);
void addLine(int bytesOffset, __int64 hexOffset);
BOOL filePrepare(LPCTSTR lpFileName);
BOOL getDataPointer();
void displayData();
void addData(direction direction);
void shiftData();
void clearGlobalParams();
int getCurrentBlock(__int64 currentScrollPos);
void wipeData();
void setScroll();
int displayErrorMessage();
//////////////////////////////////////////////////////////

HWND hScroll;				//Дескриптор контрола Scroll
SCROLLINFO scrollInfo;		//Структура скроллбара

HMENU hMenu;				//Дескриптор меню "Открыть файл"

HWND hEditOffset;			//Дескриптор контрола Edit с данными о смещении
HWND hEditBytes;			//Дескриптор контрола Edit с данными о байтах
HWND hEditText;				//Дескриптор контрола Edit с данными о прочитанном тексте

CString dataEditOffset;		//Строка с данными о смещении в контроле hEditOffset
CString dataEditBytes;		//Строка с данными о байтах в контроле hEditBytes
CString dataEditText;		//Строка с данными о прочитанном тексте в контроле hEditText

HANDLE hMapFile;			//Дескриптор маппинга файла
PBYTE pbFile;				//Указатель на начальный элемент спроецированного блока файла в памяти

__int64 fileSize;			//Общий размер обрабатываемого файла
DWORD dwBytesInBlock;		//Кол-во байт в спроецированном блоке
DWORD dwSystemGranularity;	//Гранулярность выделения памяти (64 кб)

__int64 scrollPos;			//Текущая позиция ползунка скроллбара
__int64 scrollBuffer;		//Позиция ползунка скроллбара для сравнения с текущей

int blocksInFile;			//Кол-во блоков, на которые разбит файл
int currentBlock;			//Активный в данный момент блок

__int64 blockBegin;			//Начальная граница блока памяти в файле
__int64 blockEnd;			//Конечная граница блока памяти в файле

const int appWidth{ 725 };	//Ширина окна приложения
const int appHeight{ 530 };	//Высота окна приложения