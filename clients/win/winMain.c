/* noch ein versuch */
#define STRICT
#define _WIN32_WINNT 0x0400
#define _MT
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h> 


#define PORT 7010
#define DLOCK "<ZEIBERBUDE><STOP /></ZEIBERBUDE>"
#define DULOCK "<ZEIBERBUDE><START /></ZEIBERBUDE>"

int StartWinsock(void);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam);
int parseCommand(const char *cmd);
void WindowThread(void *blub);
void LockScreen();
void UnlockScreen();
HHOOK lockHook;
const char *appName;
WNDCLASS wc;
HWND hWnd;
HWND root;
RECT rootRect;
MSG msg;
HANDLE thMut;
int locked;
unsigned long thId;
UINT nPreviousState;
OSVERSIONINFO  osvi;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR cmdLine, int cmdShow)
{
	// socket
	long rc;
	SOCKET acceptSock;
	SOCKET connSock;
	SOCKADDR_IN addr;
	char buff[256];
	const char *opt = "true";
	int ThreadNum = 1;
	thMut = CreateMutex(NULL,TRUE,NULL);
	locked = 0;
	
	root = GetDesktopWindow();
	GetWindowRect(root,&rootRect);
	//window
	appName = "zbdesk";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra    =  0;
	wc.cbWndExtra    =  0;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL,IDC_NO); //LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL; //LoadIcon(NULL, IDI_APPLICATION);
	wc.hbrBackground = CreateSolidBrush(0x00996666);
	wc.lpszClassName = appName;
	wc.lpszMenuName = NULL;
	RegisterClass(&wc);

	
	// socket
	rc = StartWinsock();
	if(rc != 0)
	{
		return 1;
	}
	acceptSock = socket(AF_INET, SOCK_STREAM,0);
	if(acceptSock == INVALID_SOCKET)
		return 1;
	memset(&addr,0,sizeof(SOCKADDR_IN));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = ADDR_ANY;
	setsockopt(acceptSock,SOL_SOCKET,SO_DONTLINGER,opt,strlen(opt));
	rc = bind(acceptSock,(SOCKADDR*)&addr,sizeof(SOCKADDR_IN));
	if(rc == SOCKET_ERROR)
		return 1;
	rc = listen(acceptSock,10);
	if(rc == SOCKET_ERROR)
		return 1;
	thId = _beginthread(WindowThread,0,&ThreadNum);
	while(1)
	{
		connSock = accept(acceptSock,NULL,NULL);
		if(connSock == INVALID_SOCKET)
			continue;
		rc = recv(connSock,buff,256,0);
		if(rc == 0)
		{
			continue;
		}
		if(rc == SOCKET_ERROR)
		{
			continue;
		}
		buff[rc] = '\0';
		parseCommand(buff);
		closesocket(connSock);
	}
	

	return 0;
}

int StartWinsock(void)
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2,0),&wsa);
}

void WindowThread(void *blub)
{
	int isLock = 0;
	hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME,appName,"Zeiberbude V2.0",
						WS_SIZEBOX,
						rootRect.left,rootRect.top,rootRect.right,rootRect.bottom,
						NULL,NULL,wc.hInstance,NULL);
	while(GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	const char logo[] = "sorry, this screen is locked";
	SIZE logoSize;
	RECT rcLogo;
	switch(msg)
		{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
		case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hDc;
				HBRUSH hBrush = CreateSolidBrush(0x00660000);

				hDc = BeginPaint(hWnd,&ps);
				{
					// rechteck
					GetClientRect(hWnd,&rc);
					rc.bottom = rc.top + 30;
					FillRect(hDc,&rc,hBrush);
					// logo
					SetTextColor(hDc,0x00FFFFFF);
					SetBkColor(hDc,0x00660000);
					GetTextExtentPoint32(hDc,logo,strlen(logo),&logoSize);
					rcLogo.left = rc.right - logoSize.cx - 5;
					rcLogo.top = rc.top + 5;
					rcLogo.bottom = rc.top + 30;
					rcLogo.right = rc.right - 5;
					DrawText(hDc,logo,strlen(logo),&rcLogo,DT_RIGHT);
				}
				EndPaint(hWnd,&ps);
				return 0;
			}
		case WM_SHOWWINDOW:
			{
				if(wParam){
					// uncomment the folowing line for win nt support
					//lockHook = SetWindowsHookEx(WH_KEYBOARD_LL,(HOOKPROC)LowLevelKeyboardProc,wc.hInstance,0);
					SystemParametersInfo (SPI_SETSCREENSAVERRUNNING, TRUE, &nPreviousState, 0);
				}
				else{
					// uncomment the folowing line for win nt support
					//UnhookWindowsHookEx(lockHook);
					SystemParametersInfo (SPI_SETSCREENSAVERRUNNING, TRUE, &nPreviousState, 0);
				}
			}
		}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK LowLevelKeyboardProc(INT nCode, WPARAM wParam, LPARAM lParam)
{
	SetForegroundWindow(hWnd);
	return 1;
}

void LockScreen()
{
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	SetForegroundWindow(hWnd);
}

void UnlockScreen()
{
	ShowWindow(hWnd,SW_HIDE);
	UpdateWindow(hWnd);
}

int parseCommand(const char *cmd){
	xmlDocPtr doc;
	xmlNodePtr cur;
	    
	doc = xmlParseMemory(cmd,strlen(cmd));
	if(doc == NULL)
		return(-1);
	cur = xmlDocGetRootElement(doc);
	if(cur == NULL){
		xmlFreeDoc(doc);
		return(-1);
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "ZEIBERBUDE"))
		return(-1);
	cur=cur->xmlChildrenNode;
	while(cur != NULL){
		if(!xmlStrcmp(cur->name, (const xmlChar *) "START")){
			UnlockScreen();
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "STOP")){
			LockScreen();
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "STATUS")){
			// status ausgeben
		}
		cur=cur->next;
	}
	return(0);
} 
