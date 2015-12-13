#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <wingdi.h>

#include <complex>

const int MAX_IT=500;
const int MAX_ABS=200;

const double mulred=0.6;
const double mulgreen=1.5;
const double mulblue=0.7;
const int side=400;
const int initScope=3;

struct Plane{
	std::complex<double> center;
	int zoom;
	Plane (double x, double y, int n) : center(x,y), zoom(n) {}
	std::complex<double> coor(int x, int y){
	return std::complex<double>((double)(x - side/2)/(double)(side/2/initScope)/zoom, (double)(y - side/2)/(double)(side/2/initScope)/zoom)+center;
	}
};

VOID KeyEventProc(KEY_EVENT_RECORD); 
VOID MouseEventProc(MOUSE_EVENT_RECORD, HWND, Plane&); 
 
COLORREF slowness(std::complex<double>);
void draw(HDC, Plane&);



int main() 
{ 
	HWND hwnd = GetConsoleWindow();
	HDC hdc = GetDC (hwnd);
	
 		 
	const char* t = "Left/right click : zoom in/out      ESC : quit"; 
	SetTextColor (hdc, RGB(255, 255, 255));
	SetBkColor(hdc, RGB(0, 0, 0));
	TextOutA (hdc, 10, side+20, t, 46);
	 
	Plane pl(0, 0, 1);
	draw(hdc, pl);
 
 
	HANDLE hStdin; 
	hStdin = GetStdHandle(STD_INPUT_HANDLE); 

	DWORD cNumRead, fdwMode, i; 
	INPUT_RECORD irInBuf[128]; 
	int counter=0;
	fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
	if(!SetConsoleMode(hStdin, fdwMode))
		ExitProcess(0);

    // Loop to read and handle the next 100 input events. 

    while (1) 
    { 
        // Wait for the events. 
 
        if (! ReadConsoleInput( 
                hStdin,      // input buffer handle 
                irInBuf,     // buffer to read into 
                128,         // size of read buffer 
                &cNumRead) ) // number of records read 
            ExitProcess(0); //ErrorExit("ReadConsoleInput"); 
 
        // Dispatch the events to the appropriate handler. 
 
        for (i = 0; i < cNumRead; i++) 
        {
            switch(irInBuf[i].EventType) 
            { 
                case KEY_EVENT: // keyboard input 
                    KeyEventProc(irInBuf[i].Event.KeyEvent); 
                    break; 
 
                case MOUSE_EVENT: // mouse input 
                    MouseEventProc(irInBuf[i].Event.MouseEvent, hwnd, pl); 
                    break; 
 
                case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing 
                    //ResizeEventProc( irInBuf[i].Event.WindowBufferSizeEvent ); 
                    break; 
 
                case FOCUS_EVENT:  // disregard focus events 
 
                case MENU_EVENT:   // disregard menu events 
                    break; 
 
                default: 
                    break; 
            } 
        }
    } 

    // Restore input mode on exit.
	// oboidutsya


    return 0; 
}


COLORREF slowness(std::complex<double> c){
	std::complex<double> z(0,0);
	int i=0;
	while (++i < MAX_IT && std::abs(z) < MAX_ABS){
		z=z*z+c;
	}
	return RGB(i*(mulred*(255.0/MAX_IT)), i*(mulgreen*(255.0/MAX_IT)), i*(mulblue*(255.0/MAX_IT)));
}


void draw(HDC hdc, Plane& pl){
	for (int i=0; i<=400; ++i)
		for (int j=0; j<=400; ++j){
			std::complex<double> c = pl.coor(i,j);
			SetPixel(hdc, i, j, slowness(c));
			if (i==0 || j==0 || i==400 || j==400)
				SetPixel(hdc, i, j, RGB(255, 255, 255));

		}
}




VOID KeyEventProc(KEY_EVENT_RECORD ker)
{

    if (ker.wVirtualKeyCode == VK_ESCAPE)
    	ExitProcess(0);
}

VOID MouseEventProc(MOUSE_EVENT_RECORD mer, HWND wh, Plane& pl)
{

    //printf("Mouse event: ");
    
    switch(mer.dwEventFlags)
    {
        case 0:

            if(mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
            {

    			POINT p;
    			GetCursorPos(&p);
				RECT r;
				GetWindowRect(wh, &r);
				int x=p.x-r.left-6;
				int y=p.y-r.top-30;
				if (!(pl.zoom*2 < 0))
					pl.zoom=pl.zoom*2;
				pl.center=pl.coor(x,y);//std::complex<double>(x,y);
				draw(GetDC(wh), pl);
				//return;
				//std::cout<<"x="<<p.x-r.left-6<<" y="<<p.y-r.top-30<<std::endl;
            }
            else if(mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
            {
                if (pl.zoom>=2)
			pl.zoom=pl.zoom/2;
		draw(GetDC(wh), pl);
            }

            break;

        default:
			
            break;
    }
}

