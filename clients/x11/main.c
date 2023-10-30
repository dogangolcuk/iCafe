/*
In der XF86Config bei den Serverflags noch ein Eintrag
Option "DontZap" "True"
um Ctrl+Alt+Backspace zu unterbinden
zeiberdube projesinden uzun süre önce geliştirilmesi durmuş.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "logo.xpm"
#include "locked.xpm"

#define PORT 7010
#define MAXMSG 512

Window root,wnd,logo,logo2;
int x,y,dW,dH;
int running = 0;
char *background = NULL;
int lockScreen(Display **dsp,Window *wnd1){
	XSetWindowAttributes attr,attr1,attr2;
	Pixmap shape,pic,lock,shp, bg_pic, bg_shp;
	int screen_number = 0;
	long win_mask = CWBackPixel|CWBorderPixel|CWOverrideRedirect;

  if(running == 1)
    return 0;
	running = 1;
	x=5;
	y=35;
	(*dsp) = XOpenDisplay(":0.0");
	if((*dsp) == NULL)
		return(-1);
	//dsp = XOpenDisplay(getenv("DISPLAY"));
	dW = XDisplayWidth(*dsp,screen_number);
	dH = XDisplayHeight(*dsp,screen_number);
	root = XRootWindow(*dsp,screen_number);

	if(background != NULL)
	  {
	    if(XpmReadFileToPixmap(*dsp,root,background,&bg_pic,&bg_shp,NULL) != XpmOpenFailed)
	      win_mask = CWBackPixmap|CWBorderPixel|CWOverrideRedirect;
	  }

	attr.background_pixel = 700;
	attr.background_pixmap = bg_pic;
	attr.border_pixel = 100;
	attr.override_redirect = True;

	(*wnd1) = XCreateWindow(*dsp, root, 0, 0, dW, dH,0,  
		 CopyFromParent, CopyFromParent,CopyFromParent,
		 win_mask
		,&attr);
	XMapRaised(*dsp,*wnd1);

	XpmCreatePixmapFromData(*dsp,*wnd1,logo_xpm,&pic,&shape,NULL);

	attr1.background_pixmap = pic;
	logo = XCreateWindow(*dsp,*wnd1,dW-150,0,150,30,0,CopyFromParent,CopyFromParent,CopyFromParent,CWBackPixmap,&attr1);
	XMapWindow(*dsp,logo);
	logo2 = XCreateSimpleWindow(*dsp, *wnd1, 0, 0, dW-150, 30, 0, 0,300);
	XMapWindow(*dsp,logo2);

	XpmCreatePixmapFromData(*dsp,*wnd1,locked_xpm,&lock,&shp,NULL);
	attr2.background_pixmap = lock;
	wnd = XCreateWindow(*dsp,*wnd1,dW/2-150,dH/2-100,300,200,0,CopyFromParent,CopyFromParent,CopyFromParent,CWBackPixmap,&attr2);
	//wnd = XCreateSimpleWindow(*dsp, *wnd1, x, y, 200, 200, 1, 0, 400);
	XMapWindow(*dsp,wnd);

	XGrabPointer(*dsp,*wnd1,False,ButtonPressMask|ButtonReleaseMask|PointerMotionMask,GrabModeAsync,GrabModeAsync,None,None,CurrentTime);
	XGrabKeyboard(*dsp,*wnd1,False,GrabModeAsync,GrabModeAsync,CurrentTime);
	XSelectInput(*dsp,*wnd1,KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|PointerMotionMask|ShiftMask|LockMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask);

	return(0);
}

int unlockScreen(Display **dsp,Window *wnd1){
	if(running == 1){
		XDestroyWindow(*dsp,*wnd1);
		XCloseDisplay(*dsp);
		running = 0;
	}
	return(0);
}

int parseCommand(const char *cmd){
	xmlDocPtr doc;
	xmlNodePtr cur;
	Display *dsp;
	Window wnd;

	//fprintf(stderr,"'%s'",cmd);
	doc = xmlParseMemory(cmd,strlen(cmd));
	if(doc == NULL)
		return(-1);
	cur = xmlDocGetRootElement(doc);
	if(cur == NULL){
		//fprintf(stderr,"empty document\n");
		xmlFreeDoc(doc);
		return(-1);
	}
	if (xmlStrcmp(cur->name, (const xmlChar *) "ZEIBERBUDE"))
		return(-1);
	//printf("Rootelement: '%s'\n",cur->name);
	cur=cur->xmlChildrenNode;
	while(cur != NULL){
		if(!xmlStrcmp(cur->name, (const xmlChar *) "START")){
			//printf("start\n");
			unlockScreen(&dsp,&wnd);
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "STOP")){
			//printf("stop\n");
			if(lockScreen(&dsp,&wnd) < 0){
				fprintf(stderr,"Cannot open Display\n");
				exit(-1);
			}
		}
		else if(!xmlStrcmp(cur->name, (const xmlChar *) "STATUS")){
			//printf("status\n");
			//printf("Price: %s\n",xmlGetProp(cur,(const xmlChar *) "PRICE"));
			//printf("Time: %s\n", xmlGetProp(cur,(const xmlChar *) "TIME"));
		}
		cur=cur->next;
	}

	return(0);
}

int make_socket (uint16_t port){
	int sock;
	struct sockaddr_in name;
	/* Create the socket. */
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		perror ("socket");
		exit (EXIT_FAILURE);
	}   
	/* Give the socket a name. */
	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sock, (struct sockaddr *) &name, sizeof (name)) < 0)
		{
			perror ("bind");
			exit (EXIT_FAILURE);
		} 
	return sock;
}


int read_from_client (int filedes){
	char buffer[MAXMSG];
	int nbytes;
	nbytes = read (filedes, buffer, MAXMSG);
	if (nbytes < 0){
		/* Read error. */
		perror ("read");
		exit (EXIT_FAILURE);
	}
	else if (nbytes == 0)
		/* End-of-file. */
		return -1;
	else{
		buffer[nbytes-1] = '\0';
		parseCommand(buffer);
		return 0;
	}
}

int main (int argc, char *argv[]){
	extern int make_socket (uint16_t port);
	int sock;
	fd_set active_fd_set, read_fd_set;
	int i;
	struct sockaddr_in clientname;
	size_t size;
	int c;
	while((c = getopt(argc, argv, "i:")) != -1)
	  {
	    switch(c)
	      {
	      case 'i':
		background = optarg;
		break;
	      }
	  }
	if(strcmp(getenv("LOGNAME"),"root") != 0){
		printf("This program must run as root!\n");
		exit(1);
	}
	/* Create the socket and set it up to accept connections. */
	sock = make_socket (PORT);
	if (listen (sock, 1) < 0){
		perror ("listen");
		exit (EXIT_FAILURE);
	}  
	/* Initialize the set of active sockets. */
	FD_ZERO (&active_fd_set);
	FD_SET (sock, &active_fd_set);
	while (1){
		/* Block until input arrives on one or more active sockets. */
		read_fd_set = active_fd_set;
		if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0){
			perror ("select");
			exit (EXIT_FAILURE);
		}
		/* Service all the sockets with input pending. */
		for (i = 0; i < FD_SETSIZE; ++i)
			if (FD_ISSET (i, &read_fd_set)){
				if (i == sock){
					/* Connection request on original socket. */
					int new;
					size = sizeof (clientname);
					new = accept (sock,(struct sockaddr *) &clientname,&size);
					if (new < 0){
						perror ("accept");
						exit (EXIT_FAILURE);
					}
					//fprintf (stderr,"Server: connect from host %s, port %hd.\n",inet_ntoa (clientname.sin_addr),ntohs (clientname.sin_port));
					FD_SET (new, &active_fd_set);
				}
				else{
					/* Data arriving on an already-connected socket. */
					if (read_from_client (i) < 0){
						close (i);
						FD_CLR (i, &active_fd_set);
					}
				}
			}
	}
}
