#include "stdafx.h"
#include "resource.h"
#include "winsock2.h" 
#pragma comment(lib,"ws2_32.lib") 
#define MAX_LOADSTRING 100
#define MAX_STRING 100
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text
#define PORT_ADDR         80 
#define QUEUE_SIZE        5 
#define WM_ASYNC_CLIENT   WM_USER+1 
#define DEFAULT_PROTOCOL  SOCK_STREAM 
#define MAX_DATA          1024 
 
int   bStartServer=0; 
BYTE  buffer[MAX_DATA]={0}; 
TCHAR log_str[MAX_DATA]={0}; 
 
char *reply="HTTP/1.0\r\nAccept: */*""\r\n\r\n"; 
char *head="<html><title>PC 18</title><body><p>"; 
char *end_head="</p></body></html>"; 
WSADATA wsaData; 
SOCKET  servsocket=INVALID_SOCKET, clientsocket=INVALID_SOCKET;
// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
BOOL    StartServer(HWND m_hWnd); 
void    StopServer(HWND m_hWnd); 
BOOL    CreateSocket(); 
BOOL    LinkSocketPort(); 
BOOL    LinkWindowSocket(HWND m_hWnd); 
BOOL    ListenSocket(); 
BOOL    StartWinSock(); 
BOOL    StopWinSock(); 
void    CloseSocket(); 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR     lpCmdLine,    int       nCmdShow)
{	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LAB5, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{return FALSE;}
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LAB5);
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
	{TranslateMessage(&msg);
	DispatchMessage(&msg);}}
	return msg.wParam;}
ATOM MyRegisterClass(HINSTANCE hInstance)
{	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LAB5);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_LAB5;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	return RegisterClassEx(&wcex);}
BOOL StartServer(HWND m_hWnd) 
{ if (!StartWinSock()) return FALSE; 
  if (!CreateSocket()) return FALSE; 
  if (!LinkSocketPort()) return FALSE; 
  if (!LinkWindowSocket(m_hWnd)) 
  return FALSE; 
  if (!ListenSocket()) return FALSE; 
  wsprintf(log_str,"Waiting for accept clients"); 
  InvalidateRect(m_hWnd,NULL,TRUE); 
  return TRUE;} 
 void StopServer(HWND m_hWnd) 
{ CloseSocket(); 
  StopWinSock(); 
  wsprintf(log_str,"Server closed socket."); 
  InvalidateRect(m_hWnd,NULL,TRUE);} 
 BOOL StartWinSock() 
{ if(WSAStartup(WINSOCK_VERSION, &wsaData)) 
{MessageBox(NULL,"Error initialize Winsock!", 
               szTitle,MB_OK|MB_ICONERROR); 
WSACleanup(); 
return FALSE;} 
return TRUE;} 
BOOL StopWinSock() 
{if(WSACleanup()) 
{MessageBox(NULL,"Error WSACleanup!",szTitle,MB_OK|MB_ICONERROR); 
return FALSE;} 
return TRUE;} 
BOOL CreateSocket() 
{servsocket = socket(PF_INET,DEFAULT_PROTOCOL,IPPROTO_TCP); 
if (servsocket==INVALID_SOCKET) 
{MessageBox(NULL,"Error create socket!",szTitle,MB_OK|MB_ICONERROR); 
return FALSE;} 
return TRUE;} 
void CloseSocket() 
{if (clientsocket!=INVALID_SOCKET) 
  {shutdown(clientsocket,SD_BOTH); 
    closesocket(clientsocket);} 
   if (servsocket!=INVALID_SOCKET) 
  { shutdown(servsocket,SD_BOTH); 
    closesocket(servsocket);}} 
BOOL LinkSocketPort() 
{ SOCKADDR_IN socketaddr; 
  socketaddr.sin_family = AF_INET; 
  socketaddr.sin_addr.s_addr  = INADDR_ANY; 
  socketaddr.sin_port = htons(PORT_ADDR); 
  if(bind(servsocket,(LPSOCKADDR)&socketaddr, 
           sizeof(socketaddr)) == SOCKET_ERROR) 
{MessageBox(NULL,"Error bind socket!",szTitle,MB_OK|MB_ICONERROR); 
return FALSE;} 
return TRUE;} 
BOOL LinkWindowSocket(HWND m_hWnd) 
{ if(WSAAsyncSelect(servsocket,m_hWnd,WM_ASYNC_CLIENT, 
              FD_ACCEPT|FD_READ|FD_CLOSE) == SOCKET_ERROR) 
{MessageBox(NULL,"Error AsyncSelect!",szTitle,MB_OK|MB_ICONERROR); 
return FALSE;} 
return TRUE;} 
BOOL ListenSocket() 
{if(listen(servsocket,QUEUE_SIZE) == SOCKET_ERROR) 
  { MessageBox(NULL,"Error listen socket!", szTitle,MB_OK|MB_ICONERROR); 
    return FALSE;} 
  return TRUE;} 
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{   HWND hWnd;
   hInst = hInstance; // Store instance handle in our global variable
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   if (!hWnd)
   {return FALSE;}
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	int i=0;
	RECT rect; 
SOCKET currentsock=INVALID_SOCKET; 
WORD   WSAEvent=0; 
TCHAR szStatus [2][MAX_STRING]={"HTTP Server is stopped.", "HTTP Server is running..."}; 
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	switch (message) 
	{		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{	case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);}
			break;
	case WM_PAINT: 
   hdc = BeginPaint(hWnd, &ps); 
   // TODO: Add any drawing code here... 
   GetClientRect(hWnd, &rect); 
   DrawText(hdc, szStatus[bStartServer], 
            strlen(szStatus[bStartServer]),&rect, DT_CENTER); 
   if (*log_str){  rect.top+=50; 
     DrawText(hdc, log_str, strlen(log_str),&rect, DT_LEFT);} 
   EndPaint(hWnd, &ps); 
   break;
case WM_RBUTTONUP: 
   bStartServer=(bStartServer+1)%2;
if (bStartServer) 
      StartServer(hWnd); 
   else 
      StopServer(hWnd); 
   InvalidateRect(hWnd,NULL,TRUE); 
   break; 
case WM_ASYNC_CLIENT: 
   // Parse the async_socket events: 
   currentsock = wParam; 
   WSAEvent = WSAGETSELECTEVENT(lParam); 
   switch (WSAEvent) { 
   case FD_CLOSE: 
      wsprintf(log_str,"Client close socket."); 
      InvalidateRect(hWnd,NULL,TRUE); 
      break; 
   case FD_READ: 
      i=recv(currentsock,(char*)buffer,MAX_DATA,0); 
      if (currentsock!=INVALID_SOCKET) { 
         buffer[i]=0; 
         wsprintf(log_str,"Read socket data.:\n%s",buffer); 
         InvalidateRect(hWnd,NULL,TRUE);} 
      wsprintf((char*)buffer,"%s%s Holovchenko Anna Igorivna KNT 621m%s", 
                reply,head,end_head); 
      send(currentsock,(char*)buffer, 
           strlen((char*)buffer),0); 
      closesocket(currentsock); 
      break; 
   case FD_ACCEPT: 
      clientsocket=accept(currentsock,NULL,NULL); 
if (clientsocket!=INVALID_SOCKET) { wsprintf(log_str,"Accept socket client."); 
         InvalidateRect(hWnd,NULL,TRUE);} 
      break;} 
   break;
		case WM_DESTROY:
			 if (bStartServer) StopServer(hWnd); 
   PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);}
   return 0;}
// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{switch (message)	
{		case WM_INITDIALOG:
				return TRUE;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{	EndDialog(hDlg, LOWORD(wParam));
				return TRUE;}
			break;}
    return FALSE;}
