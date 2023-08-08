#pragma once

#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <iomanip>
#include <atlstr.h>
#include <winbase.h>

/*������������*/
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

//��������� �������
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

HWND hScroll;				//���������� �������� Scroll
SCROLLINFO scrollInfo;		//��������� ����������

HMENU hMenu;				//���������� ���� "������� ����"

HWND hEditOffset;			//���������� �������� Edit � ������� � ��������
HWND hEditBytes;			//���������� �������� Edit � ������� � ������
HWND hEditText;				//���������� �������� Edit � ������� � ����������� ������

CString dataEditOffset;		//������ � ������� � �������� � �������� hEditOffset
CString dataEditBytes;		//������ � ������� � ������ � �������� hEditBytes
CString dataEditText;		//������ � ������� � ����������� ������ � �������� hEditText

HANDLE hMapFile;			//���������� �������� �����
PBYTE pbFile;				//��������� �� ��������� ������� ���������������� ����� ����� � ������

__int64 fileSize;			//����� ������ ��������������� �����
DWORD dwBytesInBlock;		//���-�� ���� � ��������������� �����
DWORD dwSystemGranularity;	//������������� ��������� ������ (64 ��)

__int64 scrollPos;			//������� ������� �������� ����������
__int64 scrollBuffer;		//������� �������� ���������� ��� ��������� � �������

int blocksInFile;			//���-�� ������, �� ������� ������ ����
int currentBlock;			//�������� � ������ ������ ����

__int64 blockBegin;			//��������� ������� ����� ������ � �����
__int64 blockEnd;			//�������� ������� ����� ������ � �����

const int appWidth{ 725 };	//������ ���� ����������
const int appHeight{ 530 };	//������ ���� ����������