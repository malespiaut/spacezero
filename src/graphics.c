 /*****************************************************************************
 **  This is part of the SpaceZero program
 **  Copyright(C) 2006-2012  MRevenga
 **
 **  This program is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License (version 3), or
 **  (at your option) any later version, as published by the Free Software 
 **  Foundation.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program; if not, write to the Free Software
 **  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ******************************************************************************/

/*************  SpaceZero  M.R.H. 2006-2012 ***1**************
		Author: MRevenga
		E-mail: mrevenga at users.sourceforge.net
		version 0.82 Jan 2012
****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <gtk/gtk.h>

#include "general.h"
#include "objects.h"
#include "graphics.h"
#include "ai.h"
#include "fonts.h"
#include "functions.h"
#include "shell.h"
#include "sound.h"
#include "menu.h"
#include "sectors.h"

#define DOT 1
#define LINE 2
#define CIRCLE 3


extern struct Player *players;
extern struct Parametres param;
extern int g_memused;
extern char version[64];
int gdrawmenu=1;

#if DEBUG
int debugoptions=1;
#endif

GtkWidget *win_main;
GtkWidget *win_mainmenu;
GtkWidget *winoptions;
GtkWidget *winabout;
GtkWidget *winhelp;

GtkWidget *options1;
GtkWidget *options2;
GtkWidget *options3;
GtkWidget *options4;
GtkWidget *options5;
GtkWidget *options6;
GtkWidget *options7;
GtkWidget *options8;
GtkWidget *options9;
GtkWidget *options10;
GtkWidget *options11;
GtkWidget *options12;
GtkWidget *options13;
GtkWidget *options14;
GtkWidget *options15;
GtkWidget *options16;
GtkWidget *about1;
GtkWidget *about2;
GtkWidget *help1;
GtkWidget *help2;


GdkFont *gfont;
GdkPixmap *pixmap=NULL;

GdkGC *gcolors[MAXNUMPLAYERS+3];
GdkGC *penRed=NULL;
GdkGC *penLightGreen=NULL;
GdkGC *penGreen=NULL;
GdkGC *penBlue=NULL;
GdkGC *penYellow=NULL;
GdkGC *penWhite=NULL;
GdkGC *penBlack=NULL;
GdkGC *penOrange=NULL;
GdkGC *penViolet=NULL;
GdkGC *penPink=NULL;
GdkGC *penCyan=NULL;
GdkGC *penSoftRed=NULL;
GdkGC *penGrey=NULL;

struct Keys keys;

GtkWidget *d_a;

float Distance(Object *cv,float x0,float y0);

GtkWidget *InitGraphics(char *title,char *optfile,int w,int h,struct Parametres param){



  GtkWidget *vbox;
  GtkWidget *menubar;
  GtkWidget *menuitemsave;
  GtkWidget *menuitemload;
  GtkWidget *menuitemoptions;
  GtkWidget *menuitemabout;
  GtkWidget *menuitemhelp;

  GtkWidget *vbox2;
  GtkWidget *vbox3;
  GtkWidget *vbox4;
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *hbox3;
  GtkWidget *hbox4;


  char label[164];
  char labelhelp[1088];

  GtkTooltips *tooltips;
  GdkGeometry geometry;


  win_main=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win_main),title);

#ifndef GTK12  
  gtk_window_set_resizable(GTK_WINDOW(win_main),TRUE);
#endif


  vbox=gtk_vbox_new(FALSE,0);

  d_a=gtk_drawing_area_new();

  gtk_drawing_area_size(GTK_DRAWING_AREA(d_a),w,h);  

  geometry.min_width=480;
  geometry.min_height=300;

  geometry.max_width=1680;
  geometry.max_height=1050;
  geometry.base_width=1;
  geometry.base_height=1;
  geometry.width_inc=1;
  geometry.height_inc=1;
  geometry.min_aspect=1.3;
  geometry.max_aspect=2.0;

  
  gtk_window_set_geometry_hints (GTK_WINDOW (win_main),
 				 GTK_WIDGET (d_a), 
				 &geometry,
				 GDK_HINT_MAX_SIZE |
				 GDK_HINT_MIN_SIZE |
 				 GDK_HINT_ASPECT| 
				 GDK_HINT_RESIZE_INC);


  menubar=gtk_menu_bar_new();

  /* events we need to detect */
  gtk_widget_set_events(win_main,GDK_EXPOSURE_MASK
			|GDK_LEAVE_NOTIFY_MASK
			|GDK_KEY_PRESS_MASK
			|GDK_KEY_RELEASE_MASK);
  /*			|GDK_POINTER_MOTION_MASK */
  /*			|GDK_BUTTON_PRESS_MASK */
  /*			|GDK_BUTTON_RELEASE_MASK */
  /*  			|GDK_POINTER_MOTION_HINT_MASK); */

  gtk_widget_set_events(d_a,GDK_EXPOSURE_MASK
			|GDK_LEAVE_NOTIFY_MASK
			|GDK_KEY_PRESS_MASK
			|GDK_KEY_RELEASE_MASK
			|GDK_POINTER_MOTION_MASK
			|GDK_BUTTON_PRESS_MASK
			|GDK_BUTTON_RELEASE_MASK
			|GDK_POINTER_MOTION_HINT_MASK);


  

  menuitemsave=gtk_menu_item_new_with_label("SAVE");
  gtk_signal_connect(GTK_OBJECT (menuitemsave),"activate",
		     GTK_SIGNAL_FUNC(Save),NULL);
  gtk_menu_bar_append(GTK_MENU_BAR(menubar),menuitemsave);
  
  menuitemload=gtk_menu_item_new_with_label("LOAD");
  gtk_signal_connect(GTK_OBJECT (menuitemload),"activate",
		     GTK_SIGNAL_FUNC(Load),NULL);
  gtk_menu_bar_append(GTK_MENU_BAR(menubar),menuitemload);


  /* options menu */

  winoptions=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(winoptions),"Options");
  gtk_window_set_position(GTK_WINDOW(winoptions),GTK_WIN_POS_MOUSE);
#ifndef GTK12  
  gtk_window_set_deletable(GTK_WINDOW(winoptions),FALSE);
#endif
  menuitemoptions=gtk_menu_item_new_with_label("Options");
  gtk_signal_connect(GTK_OBJECT (menuitemoptions),"activate",
		     GTK_SIGNAL_FUNC(ShowWindowOptions),optfile);

  gtk_signal_connect(GTK_OBJECT (winoptions),"destroy",
		     GTK_SIGNAL_FUNC(QuitWindow),winoptions);
  gtk_signal_connect(GTK_OBJECT (winoptions),"delete_event",
		     GTK_SIGNAL_FUNC(QuitWindow),winoptions);
 
  gtk_menu_bar_append(GTK_MENU_BAR(menubar),menuitemoptions);
  

  /*********** About menu **************/
  winabout=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(winabout),"About SpaceZero");
  gtk_window_set_position(GTK_WINDOW(winabout),GTK_WIN_POS_MOUSE);
#ifndef GTK12  
  gtk_window_set_deletable(GTK_WINDOW(winabout),FALSE);
#endif
  menuitemabout=gtk_menu_item_new_with_label("About");
  gtk_signal_connect(GTK_OBJECT (menuitemabout),"activate",
		     GTK_SIGNAL_FUNC(ShowWindow),winabout);
  
  gtk_signal_connect(GTK_OBJECT (winabout),"destroy",
		     GTK_SIGNAL_FUNC(QuitWindow),winabout);
  gtk_signal_connect(GTK_OBJECT (winabout),"delete_event",
		     GTK_SIGNAL_FUNC(QuitWindow),winabout);
  gtk_menu_bar_append(GTK_MENU_BAR(menubar),menuitemabout);
  snprintf(label,150,"\t SpaceZero %s\t\n\t Jan 2012\t\n\n\n  Copyrigth mrevenga.  \n  homepage:  http://spacezero.sourceforge.net/   ",version);
  about1=gtk_label_new(label);
  gtk_widget_show(about1);
  
  about2=gtk_button_new_with_label(" Close Window");
  gtk_widget_show(about2);
  gtk_signal_connect(GTK_OBJECT (about2),"clicked",
		     GTK_SIGNAL_FUNC(QuitWindow),winabout);


  /****** Help menu *******/
  winhelp=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(winhelp),"Help SpaceZero");
  gtk_window_set_position(GTK_WINDOW(winhelp),GTK_WIN_POS_MOUSE);
#ifndef GTK12  
  gtk_window_set_deletable(GTK_WINDOW(winhelp),FALSE);
#endif

  menuitemhelp=gtk_menu_item_new_with_label("Help");
  gtk_signal_connect(GTK_OBJECT (menuitemhelp),"activate",
		     GTK_SIGNAL_FUNC(ShowWindow),winhelp);

  gtk_signal_connect(GTK_OBJECT (winhelp),"destroy", 
		   GTK_SIGNAL_FUNC(QuitWindow),winhelp); 

  gtk_signal_connect(GTK_OBJECT (winhelp),"delete_event", 
		   GTK_SIGNAL_FUNC(QuitWindow),winhelp); 

  gtk_menu_bar_append(GTK_MENU_BAR(menubar),menuitemhelp);
 

  strcpy(labelhelp,"");

  //  snprintf(labelhelp,150,"\t SpaceZero %s\t Jun 2011\t\n",version);
  //  snprintf(labelhelp,64,"\t SpaceZero %s \t\n\n",version);
  //  strcat(labelhelp,"=====================================\n");
  strcat(labelhelp,"Manual:  http://spacezero.sourceforge.net/   ");
  strcat(labelhelp,"\n\nKeyboard Controls:\n");
  //  strcat(labelhelp,"----------------------------\n");
  strcat(labelhelp,"up/down arrows manual/automatic mode.\n");
  strcat(labelhelp,"o\t\t\tenter in order menu.\n");
  strcat(labelhelp,"Esc\t\t\tcancel actual order.\n\t\t\tclose info windows.\n");
  strcat(labelhelp,"up arrow\t\taccelerate/manual mode.\n");
  strcat(labelhelp,"left/right arrows\tturn left/right.\n");
  strcat(labelhelp,"space\t\tfire.\n");
  strcat(labelhelp,"tab\t\t\tchange to next ship.\n");
  strcat(labelhelp,"Ctrl-tab\t\tchange to previous ship.\n");
  strcat(labelhelp,"PageUp\t\tchange to next planet.\n");
  strcat(labelhelp,"PageDown\tchange to previous planet.\n");
  strcat(labelhelp,"Home\t\tchange to first ship in outer space.\n");
  strcat(labelhelp,"1 2 3\t\tchoose weapon.\n");
  strcat(labelhelp,"Ctrl-[f1f2f3f4]\tmark a ship to be selected.\n");
  strcat(labelhelp,"f1 f2 f3 f4\t\tselect a previous marked ship.\n");
  strcat(labelhelp,"f5\t\t\tshow a ship list.\n");
  strcat(labelhelp,"f6\t\t\tshow game statistics.\n");
  strcat(labelhelp,"f7\t\t\tshow game messages log.\n");
  strcat(labelhelp,"m\t\t\tshow space map.\n");
  strcat(labelhelp,"Ctrl +/-\t\tvolume up/down.\n");
  strcat(labelhelp,"Ctrl-n\t\twindow, ship mode view.\n");
  strcat(labelhelp,"Ctrl-p\t\tpause game\n");
  strcat(labelhelp,"Ctrl-s\t\tsave the game.\n");
  strcat(labelhelp,"Ctrl-l\t\t\tload the saved game.\n");
  strcat(labelhelp,"Ctrl-q\t\tquit game.\n");
  
  strcat(labelhelp,"\nIn map view:\n");
  /*  strcat(labelhelp,"-------------------\n"); */
  strcat(labelhelp,"z Z\t\t\tzoom in out.\n");
  strcat(labelhelp,"arrow keys\tmove map.\n");
  strcat(labelhelp,"space\t\tcenter map on current ship.\n");
  strcat(labelhelp,"mouse pointer\tshow coordinates.\n");
  strcat(labelhelp,"l\t\t\tshow-hide labels.\n");
  strcat(labelhelp, 
	 "left mouse \t Select the nearest ship.\nbutton\n");
  strcat(labelhelp, 
	 "right mouse \t Send the selected ships to that point.\nbutton");

#if TEST
  printf("label help size: %d (<1088)\n  ",strlen(labelhelp)); 
#endif
  if(strlen(labelhelp)>1088){
    fprintf(stderr,"ERROR InitGraphics(): cad labelhelp too long.\n");
    exit(-1);
  }

  help1=gtk_label_new(labelhelp);
  gtk_widget_show(help1);
  
  help2=gtk_button_new_with_label(" Close Window");
  gtk_widget_show(help2);
  gtk_signal_connect(GTK_OBJECT (help2),"clicked",
		     GTK_SIGNAL_FUNC(QuitWindow),winhelp);

  /****************************************/

  vbox2=gtk_vbox_new(FALSE,10);
  hbox1=gtk_hbox_new(FALSE,10);
  hbox2=gtk_hbox_new(FALSE,10);
  hbox3=gtk_hbox_new(FALSE,10);
  hbox4=gtk_hbox_new(FALSE,10);
  vbox3=gtk_vbox_new(FALSE,10);
  vbox4=gtk_vbox_new(FALSE,10);
 
  gtk_container_add(GTK_CONTAINER(winoptions),vbox2);
  gtk_container_add(GTK_CONTAINER(winabout),vbox3);
  gtk_container_add(GTK_CONTAINER(winhelp),vbox4);


  snprintf(label,128," num. of planets: (%d,%d)",MINNUMPLANETS,MAXNUMPLANETS);
  options4=gtk_label_new(label);
  gtk_widget_show(options4);
/*   options5=gtk_entry_new(); */
/*   gtk_entry_set_text(GTK_ENTRY(options5),"30"); */
/*   gtk_entry_set_width_chars(GTK_ENTRY(options5),4); */
/*   gtk_widget_show(options5); */

  options1=gtk_check_button_new_with_label("Known Universe");
  gtk_widget_show(options1);
  gtk_signal_connect(GTK_OBJECT (options1),"toggled",
 		     GTK_SIGNAL_FUNC(PrintMessage),"option1"); 


  options2=gtk_check_button_new_with_label("Music OFF");
  gtk_widget_show(options2);
  gtk_signal_connect(GTK_OBJECT (options2),"toggled",
 		     GTK_SIGNAL_FUNC(PrintMessage),"option2"); 

  options3=gtk_check_button_new_with_label("Sound OFF");
  gtk_widget_show(options3);
  gtk_signal_connect(GTK_OBJECT (options3),"toggled",
 		     GTK_SIGNAL_FUNC(PrintMessage),"option3"); 
  
  snprintf(label,128," num. of planets: (%d,%d)",MINNUMPLANETS,MAXNUMPLANETS);
  options4=gtk_label_new(label);
  gtk_widget_show(options4);
  options5=gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(options5),"20");
#ifndef GTK12  
  gtk_entry_set_width_chars(GTK_ENTRY(options5),4);
#endif  
  gtk_widget_show(options5);

  snprintf(label,128," num. of players: (%d,%d)",MINNUMPLAYERS,MAXNUMPLAYERS);
  options6=gtk_label_new(label);
  gtk_widget_show(options6);
  options7=gtk_entry_new();
#ifndef GTK12  
  gtk_entry_set_width_chars(GTK_ENTRY(options7),4);
#endif 
  gtk_entry_set_text(GTK_ENTRY(options7),"2");
  gtk_widget_show(options7);


  snprintf(label,128," Universe Size: (%d,%d)",MINULX,MAXULX);
  options8=gtk_label_new(label);
  gtk_widget_show(options8);
  options9=gtk_entry_new();
#ifndef GTK12  
    gtk_entry_set_width_chars(GTK_ENTRY(options9),8);
#endif
  gtk_entry_set_text(GTK_ENTRY(options9),"100000");
  gtk_widget_show(options9);


  options10=gtk_button_new_with_label(" Set Default");
  gtk_widget_show(options10);
  gtk_signal_connect(GTK_OBJECT (options10),"clicked",
		     GTK_SIGNAL_FUNC(SetDefaultOptions),"set default");

  options11=gtk_button_new_with_label(" Save ");
  gtk_widget_show(options11);
  gtk_signal_connect(GTK_OBJECT (options11),"clicked",
		     GTK_SIGNAL_FUNC(SaveWindowOptions),optfile);

  options12=gtk_button_new_with_label(" Cancel ");
  gtk_widget_show(options12);
  gtk_signal_connect(GTK_OBJECT (options12),"clicked",
		     GTK_SIGNAL_FUNC(QuitWindow),winoptions);


  snprintf(label,128," Changes will take effect after rerun spacezero.\n (sound and music when save)");
  options13=gtk_label_new(label);
  gtk_widget_show(options13);


  options14=gtk_check_button_new_with_label("Cooperative mode");
  gtk_widget_show(options14);
  gtk_signal_connect(GTK_OBJECT (options14),"toggled",
 		     GTK_SIGNAL_FUNC(PrintMessage),"option14"); 

  options15=gtk_check_button_new_with_label("Computer cooperative mode");
  gtk_widget_show(options15);
  gtk_signal_connect(GTK_OBJECT (options15),"toggled",
 		     GTK_SIGNAL_FUNC(PrintMessage),"option15"); 

  options16=gtk_check_button_new_with_label("Queen mode");
  gtk_widget_show(options16);
  gtk_signal_connect(GTK_OBJECT (options16),"toggled",
 		     GTK_SIGNAL_FUNC(PrintMessage),"option16"); 




  gtk_box_pack_start(GTK_BOX(vbox2),options13,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),options1,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),options2,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),options3,FALSE,FALSE,0);

  gtk_box_pack_start(GTK_BOX(vbox2),hbox1,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),hbox2,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),hbox3,FALSE,FALSE,0);
/*   gtk_box_pack_start(GTK_BOX(vbox2),options14,FALSE,FALSE,0); */


  gtk_box_pack_start(GTK_BOX(vbox2),options14,FALSE,FALSE,0);
/*   gtk_box_pack_start(GTK_BOX(vbox2),options17,FALSE,FALSE,0); */
  gtk_box_pack_start(GTK_BOX(vbox2),options15,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),options16,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox2),hbox4,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox1),options4,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox1),options5,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox2),options6,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox2),options7,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox3),options8,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox3),options9,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(hbox4),options11,FALSE,FALSE,0);/*save */
  gtk_box_pack_start(GTK_BOX(hbox4),options10,FALSE,FALSE,0);/*load */
  gtk_box_pack_start(GTK_BOX(hbox4),options12,FALSE,FALSE,0);/*quit options*/
  //    gtk_box_pack_start(GTK_BOX(vbox2),vbox3,FALSE,FALSE,0);/* about menu */ /* HERE why must be commented*/
  gtk_box_pack_start(GTK_BOX(vbox3),about1,FALSE,FALSE,0); /* about text*/
  gtk_box_pack_start(GTK_BOX(vbox3),about2,FALSE,FALSE,0);/*quit about */

  gtk_box_pack_start(GTK_BOX(vbox4),help1,FALSE,FALSE,0); /* help text*/
  gtk_box_pack_start(GTK_BOX(vbox4),help2,FALSE,FALSE,0);/*quit help */
  /* -- options menu */



  gtk_container_add(GTK_CONTAINER(win_main),vbox);
  /*  gtk_box_pack_start(GTK_BOX(vbox),vbox2,FALSE,TRUE,0); */

  gtk_signal_connect(GTK_OBJECT(win_main),"destroy",
		     GTK_SIGNAL_FUNC(QuitGraphics),NULL);
  
  gtk_signal_connect(GTK_OBJECT(d_a),"expose_event",
		     GTK_SIGNAL_FUNC(expose_event),NULL);
/*    gtk_signal_connect(GTK_OBJECT(d_a),"size_request",    */
/*      		     GTK_SIGNAL_FUNC(SizeRequest),NULL);    */
  gtk_signal_connect(GTK_OBJECT(d_a),"configure_event",
		     GTK_SIGNAL_FUNC(configure_event),NULL);
  
  gtk_signal_connect(GTK_OBJECT (win_main),"key_press_event",
		     GTK_SIGNAL_FUNC(key_press),NULL);
  
  gtk_signal_connect(GTK_OBJECT (win_main),"key_release_event",
		     GTK_SIGNAL_FUNC(key_release),NULL);
  
  gtk_signal_connect(GTK_OBJECT (win_main),"focus_in_event",
		     GTK_SIGNAL_FUNC(GotFocus),NULL);
  gtk_signal_connect(GTK_OBJECT (win_main),"focus_out_event",
		     GTK_SIGNAL_FUNC(LostFocus),NULL);

  /* mouse */
  gtk_signal_connect(GTK_OBJECT (d_a),"motion_notify_event", 
 		     GTK_SIGNAL_FUNC(motion_notify),NULL); 
  gtk_signal_connect(GTK_OBJECT (d_a),"button_press_event",
		     GTK_SIGNAL_FUNC(button_press),NULL);
  gtk_signal_connect(GTK_OBJECT (d_a),"button_release_event",
		     GTK_SIGNAL_FUNC(button_release),NULL);

  gtk_box_pack_start(GTK_BOX(vbox),menubar,FALSE,TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),d_a,TRUE,TRUE,0);
  
  gtk_widget_show(menubar);
  gtk_widget_show(menuitemsave);
  gtk_widget_show(menuitemload);
  gtk_widget_show(menuitemoptions);
  gtk_widget_show(menuitemabout);
  gtk_widget_show(menuitemhelp);
  gtk_widget_show(d_a);
  gtk_widget_show(win_main);
  /*  gtk_widget_show(winoptions); */
  gtk_widget_show(vbox);
  gtk_widget_show(vbox2);
  gtk_widget_show(hbox1);
  gtk_widget_show(hbox2);
  gtk_widget_show(hbox3);
  gtk_widget_show(hbox4);
  gtk_widget_show(vbox3);
  gtk_widget_show(vbox4);

#ifndef GTK12
  gtk_window_resize(GTK_WINDOW(win_main),w,h);
#endif



  /* tooltips */
  tooltips = gtk_tooltips_new ();
  gtk_tooltips_set_tip (tooltips, options1, "All The Universe is known by all players.", NULL);

  tooltips = gtk_tooltips_new ();
  gtk_tooltips_set_tip (tooltips, options14, "All human players belongs to the same team.", NULL);

  tooltips = gtk_tooltips_new ();
  gtk_tooltips_set_tip (tooltips, options15, "All computer players belongs to the same team.", NULL);

  tooltips = gtk_tooltips_new ();
  gtk_tooltips_set_tip (tooltips, options16, "If the Queen ship is destroyed, all player ships are destroyed. GAME OVER.", NULL);


  /* --tooltips */


  /******** colors **********/
  penRed= GetPen(NewColor(65535,0,0),pixmap);
  penGreen= GetPen(NewColor(0,65535,0),pixmap);
  penLightGreen= GetPen(NewColor(40000,65535,40000),pixmap);
  penBlue= GetPen(NewColor(0,29325,65535),pixmap);
  penYellow= GetPen(NewColor(65535,65535,0),pixmap);
  penWhite= GetPen(NewColor(65535,65535,65535),pixmap);
  penBlack= GetPen(NewColor(0,0,0),pixmap);
  
  /*255 165   0 */
  penOrange= GetPen(NewColor(65535,42405,0),pixmap);
  /*238 130 238 */
  penViolet= GetPen(NewColor(61166,33410,61166),pixmap);
  /*255 192 203 */
  penPink= GetPen(NewColor(65535,49344,52171),pixmap);
  penCyan= GetPen(NewColor(0,65535,65535),pixmap);
  penSoftRed= GetPen(NewColor(20000,0,0),pixmap);
  penGrey= GetPen(NewColor(40000,40000,40000),pixmap);

  gcolors[0]=penWhite;
  gcolors[1]=penGreen;
  gcolors[2]=penYellow;
  gcolors[3]=penRed;
  gcolors[4]=penBlue;
  gcolors[5]=penOrange;
  gcolors[6]=penViolet;
  gcolors[7]=penPink;
  gcolors[8]=penCyan;
  gcolors[9]=penLightGreen; /* penWhite */
  gcolors[10]=penSoftRed;

  /********* --colors *********/

  /********* fonts **********/

  gfont=InitFonts(param.font);
  if(gfont==NULL){
    GameParametres(SET,GPANEL,PANEL_HEIGHT);
  }
  else{
    GameParametres(SET,GPANEL,2*gdk_text_height(gfont,"pL",2));
  }
  GameParametres(SET,GHEIGHT,GameParametres(GET,GHEIGHT,0)-GameParametres(GET,GPANEL,0));

  /********* --fonts **********/

  return(d_a);
}

gint QuitGraphics(GtkWidget *widget,gpointer gdata){

  gtk_main_quit();
  
  return FALSE;
}


#ifndef GTK12

gint expose_event(GtkWidget *widget, GdkEventExpose *event){
  /* version 01 06Feb2011*/

  GdkRectangle *rects;
  int n_rects;
  int i;

  static int cont=0; 

  gdk_region_get_rectangles (event->region, &rects, &n_rects);
  //  printf("espose event %d %d, %d %d \n",cont,n_rects,event->area.width,event->area.height);

  for (i = 0; i < n_rects; i++)
    {
      /* Repaint rectangle: (rects[i].x, rects[i].y), 
      *                    (rects[i].width, rects[i].height)
       */

      //      printf("\t %d %d %d \n",i,rects[i].width,rects[i].height);
      gdk_draw_pixmap(widget->window,
		      widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		      pixmap,
		      rects[i].x,rects[i].y,
		      rects[i].x,rects[i].y,
		      rects[i].width,rects[i].height);

    }

  g_free (rects);
  cont++;
  return FALSE;
}

#else
gint expose_event(GtkWidget *widget, GdkEventExpose *event){
  /* version 00 */ 
  static int cont=0; 

/*   printf("espose event %d %d, %d %d \n",GetTime(),cont,event->area.width,event->area.height); */

  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		  pixmap,
		  event->area.x,event->area.y,
		  event->area.x,event->area.y,
		  -1,-1);//event->area.width,event->area.height);
  
  cont++;
  return FALSE;
}
#endif



gint configure_event(GtkWidget *widget, GdkEventConfigure *event){
  /* The window has been resized */  

  gdrawmenu=TRUE;
  if(pixmap){
    gdk_pixmap_unref(pixmap);
  }
  pixmap=gdk_pixmap_new(widget->window,
			widget->allocation.width,
			widget->allocation.height,
			-1);

  GameParametres(SET,GHEIGHT,widget->allocation.height-GameParametres(GET,GPANEL,0)); 
  GameParametres(SET,GWIDTH,widget->allocation.width); 
  
  return TRUE;
}

gint SizeRequest(GtkWidget *widget, GdkEventConfigure *event){
  /* The window has been resized */  

  /* gtk_widget_set_size_request (d_a,widget->allocation.width, */
  /* 			       widget->allocation.height); */
  printf("RESIZE d_a: %d %d  \n",widget->allocation.width,
			       widget->allocation.height); 
  return TRUE;
}


gint GotFocus(GtkWidget *widget,gpointer data){
  keys.ctrl=FALSE;
  return(0);
}

gint LostFocus(GtkWidget *widget,gpointer data){
/*   SetDefaultKeyValues(&keys); */
  keys.ctrl=FALSE;
  return(0);
}

gint button_press(GtkWidget *widget,GdkEventButton *event){
  /* printf("mouse press\n");  */
  switch(event->button){
  case 1:
    keys.mleft=TRUE;
    keys.mright=FALSE;
    break;
  case 2:
    break;
  case 3:
    keys.mright=TRUE;
    keys.mleft=FALSE;
    break;
  default:
    break;
  }

  return(0);
}

gint button_release(GtkWidget *widget,GdkEventButton *event){
  static int lasttime=0;
  int time;


  time=GetTime();
  /* printf("mouse release %d %d\n",time,lasttime);   */
  if(time-lasttime<6){
    keys.mdclick=TRUE;
  }
  lasttime=time;

  keys.mleft=FALSE;
  keys.mright=FALSE;
  return(0);
}

gint motion_notify(GtkWidget *widget,GdkEventMotion *event){
  int x,y;
  GdkModifierType state;
 
  if(event->is_hint){
    gdk_window_get_pointer(event->window,&x,&y,&state);

  } 
  else{
    x=event->x;
    y=event->y;
    state=event->state;
  }
  
  MousePos(SET,&x,&y);
  return(TRUE);
}

void MousePos(int order,int *x,int *y){
  static int x0=0,y0=0;

  switch(order){
  case SET:
    x0=*x;
    y0=*y;
    break;
  case GET:
    *x=x0;
    *y=y0;
    break;
  default:
    break;
  }
}


void key_press(GtkWidget *widget,GdkEventKey *event,gpointer data){

  guint key,keyval;
  
  //  g_print("%d, %s, %u\n",event->keyval,event->string,gdk_keyval_to_unicode(event->keyval));   
/*   g_print("%d, %s, %s, %d\n",event->keyval,event->string,gdk_keyval_name(event->keyval), */
/* 	  gdk_keyval_from_name(gdk_keyval_name(event->keyval)));  */
  CountKey(1);
  gdrawmenu=TRUE;


  switch(event->keyval){
  case 65288: /* BackSpace */ 
  case 65293: /* enter */
  case 65421: /* enter */
    break;
  case 65470: /* F1 key */
    key=70;
    Keystrokes(ADD,&key,NULL); /* F */
    key=49;
    Keystrokes(ADD,&key,NULL); /* 1 */
    break;
  case 65471: /* F2 key */
    key=70;
    Keystrokes(ADD,&key,NULL);
    key=50;
    Keystrokes(ADD,&key,NULL);
    break;
  case 65472: /* F3 key */
    key=70;
    Keystrokes(ADD,&key,NULL);
    key=51;
    Keystrokes(ADD,&key,NULL);
    break;
  case 65473: /* F4 key */
    key=70;
    Keystrokes(ADD,&key,NULL);
    key=52;
    Keystrokes(ADD,&key,NULL);
    break;
  default:
    Keystrokes(ADD,&event->keyval,NULL);
    break;
  }
  
  keyval=event->keyval; /* upper to lowercase */
  if(event->keyval>64 && event->keyval<91){
    keyval=event->keyval+32;
  }

  if(keyval==keys.fire.value){
    keys.fire.state=TRUE;
  }
  if(keyval==keys.turnleft.value){
    keys.turnleft.state=TRUE;
  }
  if(keyval==keys.turnright.value){
    keys.turnright.state=TRUE;
  }
  if(keyval==keys.accel.value){
    keys.accel.state=TRUE;
  }
  if(keyval==keys.manualmode.value){
    keys.manualmode.state=TRUE;
  }
  if(keyval==keys.automode.value){
    keys.automode.state=TRUE;
  }

  if(keyval==keys.map.value){
    keys.map.state=TRUE;
  }
  if(keyval==keys.order.value){
    keys.order.state=TRUE;
    keys.g=FALSE;
    keys.s=FALSE;
    keys.p=FALSE;
    keys.t=FALSE;
    keys.b=FALSE;
    keys.r=FALSE;
  }

  switch (event->keyval){
  case 65360:
    keys.home=TRUE;
    break;
  case 65366:
    keys.Pagedown=TRUE;
    break;
  case 65365:
    keys.Pageup=TRUE;
    break;
  case 65288:
    keys.back=TRUE;
    break;
  case 65289:
    keys.tab=TRUE;
    break;
  case 65293:
  case 65421:
    keys.enter=TRUE;
    break;
  case 65361:
    keys.left=TRUE;
    break;
  case 65362:
    keys.up=TRUE;
    break;
  case 65363:
    keys.right=TRUE;
    break;
  case 65364:
    keys.down=TRUE;
    break;
  case 65307:
    keys.esc=TRUE;
    break;
  case 65470:
    keys.f1=TRUE;
    break;
  case 65471:
    keys.f2=TRUE;
    break;
  case 65472:
    keys.f3=TRUE;
    break;
  case 65473:
    keys.f4=TRUE;
    break;
  case 65474:
    if(keys.f5==TRUE)
      keys.f5=FALSE;
    else{
      keys.f5=TRUE;
      keys.f9=FALSE;
    }
    break;
  case 65475:
    if(keys.f6==TRUE)
      keys.f6=FALSE;
    else
      keys.f6=TRUE;
    break;
  case 65476:

    if(keys.f7==TRUE)
      keys.f7=FALSE;
    else{
      keys.f7=TRUE;
      keys.f5=FALSE;
    }
    break;
  case 65477:
    keys.f8=TRUE;
    break;
  case 65478:
    keys.f9=TRUE;
    break;
  case 65479:
    keys.f10=TRUE;
    break;
  case 65505:
  case 65506:
    keys.may=TRUE;
    break;
  case 65507:
  case 65508:
    keys.ctrl=TRUE;
    break;
  case 32:
    keys.centermap=TRUE;
    break;
  case 43:
  case 65451:
    keys.plus=TRUE;
    break;
  case 45:
  case 65453:
    keys.minus=TRUE;
    break;
  case 48:
  case 65456:
    keys.number[0]=TRUE;
    break;
  case 49:
  case 65457:
    keys.number[1]=TRUE;
    break;
  case 50:
  case 65458:
    keys.number[2]=TRUE;
    break;
  case 51:
  case 65459:
    keys.number[3]=TRUE;
    break;
  case 52:
  case 65460:
    keys.number[4]=TRUE;
    break;
  case 53:
  case 65461:
    keys.number[5]=TRUE;
    break;
  case 54:
  case 65462:
    keys.number[6]=TRUE;
    break;
  case 55:
  case 65463:
    keys.number[7]=TRUE;
    break;
  case 56:
  case 65464:
    keys.number[8]=TRUE;
    break;
  case 57:
  case 65465:
    keys.number[9]=TRUE;
    break;
  case 65: /* a */
  case 97:
    break;
  case 66:
  case 98:
    if(keys.order.state==TRUE)
      keys.b=TRUE;
    break;
  case 68:
  case 100:
    if(keys.d==TRUE)
      keys.d=FALSE;
    else
      keys.d=TRUE;
    break;
  case 69:
  case 101: /*e */
    keys.e=TRUE;
    break;
  case 71:
  case 103:
    keys.g=TRUE;
    break;
  case 104:
    break;
  case 76:
  case 108:
    keys.l=TRUE;
    break;
  case 77:  /* m */
  case 109:  
    break;
  case 78:
  case 110:
    keys.n=TRUE;
    break;
  case 79: /* o */
  case 111:
    break;  
  case 80:
  case 112:
    keys.p=TRUE;
    break;
  case 81:
  case 113:
    keys.q=TRUE;
    break;
  case 82:
  case 114:
    keys.r=TRUE;
    break;
  case 83:
  case 115:
    keys.s=TRUE;
    break;
  case 84:
  case 116:
    if(keys.order.state==TRUE){
      keys.t=TRUE;
    }else{
      if(keys.trace==TRUE)
	keys.trace=FALSE;
      else
	keys.trace=TRUE;
    }
    break;
  case 85:
  case 117:
    keys.u=TRUE;
    break;
  case 87:
  case 119:
    keys.w=TRUE;
    break;
  case 88:
  case 120:
    keys.x=TRUE;
    break;
  case 89:
  case 121:
    keys.y=TRUE;
    break;

  case 90:  /* z Z */
  case 122:
    keys.z=TRUE;
    break;
  default:
    break;
  }
  if(keys.order.state==FALSE){
    /* keys.enter=FALSE; */
    keys.b=FALSE;
  }
}


void key_release(GtkWidget *widget,GdkEventKey *event,gpointer data){
  guint keyval;

  keyval=event->keyval; /* upper to lowercase */
  if(event->keyval>64 && event->keyval<91){
    keyval=event->keyval+32;
  }

  if(keyval==keys.fire.value){
    keys.fire.state=FALSE;
  }
  if(keyval==keys.turnleft.value){
    keys.turnleft.state=FALSE;
  }
  if(keyval==keys.turnright.value){
    keys.turnright.state=FALSE;
  }
  if(keyval==keys.accel.value){
    keys.accel.state=FALSE;
  }
  if(keyval==keys.automode.value){
    keys.automode.state=FALSE;
  }
  if(keyval==keys.manualmode.value){
    keys.manualmode.state=FALSE;
  }
  if(keyval==keys.map.value){
    keys.map.state=FALSE;
  }


  gdrawmenu=TRUE;
  switch (event->keyval){
  case 65360:
    keys.home=FALSE;
    break;
  case 65366:
    keys.Pagedown=FALSE;
    break;
  case 65365:
    keys.Pageup=FALSE;
    break;

  case 65288:
    keys.back=FALSE;
    break;
  case 65307:
    keys.esc=FALSE;
    break;
  case 65289:
    keys.tab=FALSE;
    break;
  case 65362:
    keys.up=FALSE;
    break;
  case 65364:
    keys.down=FALSE;
    break;
  case 65363:
    keys.right=FALSE;
    break;
  case 65361:
    keys.left=FALSE;
    break;
  case 65505:
  case 65506:
    keys.may=FALSE;
    break;
  case 65470:
    keys.f1=FALSE;
    break;
  case 65471:
    keys.f2=FALSE;
    break;
  case 65472:
    keys.f3=FALSE;
    break;
  case 65473:
    keys.f4=FALSE;
    break;
  case 65476:
    //    keys.f7=FALSE;
    break;
  case 65477:
    keys.f8=FALSE;
    break;
  case 65478:
    keys.f9=FALSE;
    break;
  case 65479:
    keys.f10=FALSE;
    break;

  case 65507:
  case 65508:
    keys.ctrl=FALSE;
    break;
  case 32://keys.space.value: //32
    keys.centermap=FALSE;
    break;

  case 43:
  case 65451:
    keys.plus=FALSE;
    break;
  case 45:
  case 65453:
    keys.minus=FALSE;
    break;

  case 65: /* a */
  case 97:
    break;
  case 90:
  case 122:
    keys.z=FALSE;
    break;
  case 76:
  case 108:
    keys.l=FALSE;
    break;
  case 77: /* m */
  case 109:
    break;
  case 78:
  case 110:
    keys.n=FALSE;
    break;
  case 80:
  case 112:
    keys.p=FALSE;
    break;
  case 81:
  case 113:
    keys.q=FALSE;
    break;
  case 83:
  case 115:
    keys.s=FALSE;
    break;
  case 88:
  case 120:
    keys.x=FALSE;
    break;
  case 89:
  case 121:
    keys.y=FALSE;
    break;
  default:
    break;
  }
}


GdkColor *NewColor(int red,int green,int blue){
  
  GdkColor *c=(GdkColor *)malloc(sizeof(GdkColor));
  g_memused+=sizeof(GdkColor);
  if(c==NULL){
    fprintf(stderr,"ERROR in malloc Newcolor()\n");
    exit(-1);
  }
  
  c->red=red;
  c->green=green;
  c->blue=blue;
  gdk_color_alloc(gdk_colormap_get_system(),c);
  
  return(c);
  
}

GdkGC *GetPen(GdkColor *c,GdkPixmap *pixmap){
  
  GdkGC *gc;
  
  gc=gdk_gc_new(pixmap);
  gdk_gc_set_foreground(gc,c);
  
  return(gc);
}


void WriteCad(GdkPixmap *pixmap,char *cad,int x,int y,GdkGC *color){
  char point[32];

  strncpy(point,cad,32);
  DrawString(pixmap,gfont,color,x,y,point);
}


int DrawObjs(GdkPixmap *pixmap,struct HeadObjList *lhc,struct Habitat habitat,Object *cv,Vector r_rel){
  /*
    version 0.1
    Draw all visible objects in pixmap
    returns:
    the number of object drawed.
   */  
  GdkGC *gc;
  GdkGC *gcexp;
  struct ObjList *ls;
  struct HeadObjList hol;
  Object *lobj=NULL;
  int x,y,r;
  int i,n=0;
  float x0,y0,x1,y1,a;
  float x_0,y_0,x_1,y_1;
  float rcosa,rsina;

  /* last to draw (cv)*/
  int lx=0,ly=0;
  int lsw=0;
  int cont=0;
  int gwidth,gheight;
  float xr,yr;
  float sx,sy;
  /*  DrawShip(pixmap,gc,x,y,a,r,ls->obj->state,ls->obj->subtype); */

  float s[]={6,
	     3,0,
	     2,-1,
	     -2,-1,
	     -2,1,
	     2,1,
	     3,0};
  
  
  if(cv==NULL)return(0);

  if(0){
    if(cv->in==NULL)n=0;
    else{
      n=(cv->in->id);
    }
    /* check */
    
    if(n<0||n>GameParametres(GET,GNPLANETS,0)){
      fprintf(stderr,"ERROR in DrawObjs() n=%d\n",n);
      exit(-1);
    }
    
    hol=lhc[n];
    
    if(hol.n==0){
      return(cont);
    }
  }

  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);

  sx=(float)gwidth/LXFACTOR;
  sy=(float)gheight/LYFACTOR;

  gc=penWhite;
  gcexp=penRed;

  xr=r_rel.x-gwidth/2;
  yr=r_rel.y-gheight/2;

  ls=lhc->next;
  

  while (ls!=NULL){
    if(ls->obj->type==SHIP && ls->obj->ttl<MINTTL){
      ls=ls->next;continue;
    }
    if(ls->obj->state<=0){ls=ls->next;continue;}
    if(ls->obj->habitat!=habitat.type){ls=ls->next;continue;}

    if(cv->in!=ls->obj->in){ls=ls->next;continue;}
    if(cv->habitat==H_SHIP){ls=ls->next;continue;}
    
    x=ls->obj->x;
    y=ls->obj->y;

    if(ls->obj->habitat==H_PLANET){
      x*=sx;
      y*=sy;
      if(ls->obj->type==SHIP){
	y+=ls->obj->radio+1;
      }
    }


    if(ls->obj->habitat==H_SPACE){
      x-=xr;
      y-=yr;
    }
    r=ls->obj->radio;

    if(x+r<0 || y+r<0 || x-r>gwidth || y-r>gheight){ 
      ls=ls->next;
      continue;
    } 
    a=ls->obj->a;  

    
    /*    printf("type: %d id:%d\n",ls->obj->type,ls->obj->id); */
    if(ls->obj->player > MAXNUMPLAYERS+2){
      fprintf(stderr,"ERROR in DrawObjs(): Too much players\n");
      fprintf(stderr,"\tid: %d player: %d\n",ls->obj->id,ls->obj->player);
      exit(-1);
    }
    
    cont++;
    switch(ls->obj->type){
    case PROJECTILE:
      switch(ls->obj->subtype){
      case SHOT1:
	gdk_draw_point(pixmap,penWhite,x+1,gheight-y);
	gdk_draw_point(pixmap,penWhite,x-1,gheight-y);
	gdk_draw_point(pixmap,penWhite,x,gheight-(y+1));
	gdk_draw_point(pixmap,penWhite,x,gheight-(y-1));
	/*	g_print("tiro:%f %f\n",ls->obj->x,ls->obj->y); */
	break;
      case MISSILE:/*SHOT3: */
	gc=gcolors[players[ls->obj->player].color];

	rcosa=r*cos(a);
	rsina=r*sin(a);
		
	n=s[0];
	for(i=0;i<s[0]-1;i++){
	  x0=s[2*i+1];
	  y0=s[2*i+2];
	  x1=s[2*i+3];
	  y1=s[2*i+4];
	  
	  x_0=x0*rcosa-y0*rsina;
	  y_0=x0*rsina+y0*rcosa;
	  
	  x_1=x1*rcosa-y1*rsina;
	  y_1=x1*rsina+y1*rcosa;
	  
	  gdk_draw_line(pixmap,gc,
			x+x_0,gheight-(y+y_0),
			x+x_1,gheight-(y+y_1));
	  
	}

	break;
      case LASER:/*SHOT4: */

	rcosa=r*cos(a);
	rsina=r*sin(a);
	
	x0=x-rcosa;
	x1=x+rcosa;
	y0=y-rsina;
	y1=y+rsina;

	gdk_draw_line(pixmap,penBlue,
		      x0,gheight-(y0),
		      x1,gheight-(y1));
	/*	printf("%d %f %f %d\n",x,x0,x1,r); */
	break;
      case EXPLOSION:
	//	gcexp=gcolors[-(ls->obj->id)%10];
	//	gdk_draw_point(pixmap,gcexp,x,gheight-y);
	//	gdk_draw_rectangle(pixmap,gcexp,TRUE,x-2,gheight-y-2,4,4);
	gcexp=gcolors[players[ls->obj->player].color];
	gdk_draw_rectangle(pixmap,gcexp,TRUE,x-1,gheight-y-1,3,3);
	break;
      default:
	fprintf(stderr,"ERROR: DrawObjs() not known\n");
	break;
      }
      break;
      
    case SHIP:
      switch(ls->obj->subtype){
      case SHIP1:
      case SHIP2:
      case SHIP3:
      case SHIP4:
      case SATELLITE:
      case TOWER:	
      case PILOT:
	if(ls->obj->habitat==H_PLANET && ls->obj==cv && ls->obj->selected==TRUE){
	  lx=x;
	  ly=y;
	  lobj=ls->obj;
	  lsw=1;
	}
	else{
	  gc=gcolors[players[ls->obj->player].color];
	  if(ls->obj->selected==TRUE && ls->obj!=cv && ls->obj->player==cv->player)gc=penGrey;
	  DrawShip(pixmap,gc,x,y,ls->obj);
	}
	
	break;
      default:
	g_print("ERROR DrawObjs(1) %d %d %d\n",ls->obj->id,ls->obj->type,ls->obj->subtype);
	exit(-1);
	break;
      }
      break;
      
    case ASTEROID:
      DrawAsteroid(pixmap,x,y,ls->obj);
      break;
    case PLANET:

      DrawPlanet(pixmap,x,y,ls->obj->radio);

      break;
    case TRACKPOINT:
      break;
    case TRACE:
      gdk_draw_point(pixmap,penWhite,x,gheight-y);
      break;
    default:
      g_print("ERROR DrawObjs(2)%d %d %d %d %d\n",ls->obj->id,ls->obj->type,ls->obj->subtype,ls->obj->player,n);
      //      exit(-1);
      break;
    }   
    ls=ls->next;
  }

  if(lsw){
    DrawShip(pixmap,penWhite,lx,ly,lobj);
  }

 return(cont);
}


void DrawShip(GdkPixmap *pixmap,GdkGC *gc,int x,int y,Object *obj){
  /*
    Draw a ship with the position and parameters gived in arguments.
  */  

  GdkGC *gc2;
  int sw=0,color;
  int gheight;
  float x0,y0,x1,y1,x2,y2;
  float x_0,y_0,x_1,y_1,x_2,y_2;
  float rcosa,rsina;
  static float s1[]={3,-1,-0.6,1,0,-1,0.6};  /*SHIP1 EXPLORER*/
  static float s2[]={5,-0.2,0,-1,-0.6,1,0,-1,0.6,-0.2,0}; /* SHIP2 */
  static float s3[]={12,            /* number of points    SHIP3 FIGHTER */
		     0.866,0.5,     /* pair of points */
		     0.5,0.866,
		     -0.5,0.866,
		     -0.866,0.5,
		     0,0,
		     -0.866,-0.5,
		     -0.5,-0.866,
		     0,-0.866,
		     0,0.866,
		     0,-0.866,
		     0.5,-0.866,
		     0.866,-0.5};
  static float s4[]={12,                 /* SHIP4 QUEEN */
		     1,0,
		     -0.5,0.5,
		     -1,0.5,
		     -0.5,0.25,
		     -0.5,-0.25,
		     -1,-0.5,
		     -0.5,-0.5,
		     1,0,
		     0.55,0,
		     -0.25,0.25,
		     -0.25,-0.25,
		     0.55,0};
  /*  static float s5[]={6, */
  /* 	      0.625,0, */
  /* 	      0.375,-0.1875, */
  /* 	      -0.375,-0.1875, */
  /* 	      -0.375,0.1875, */
  /* 	      0.375,0.1875, */
  /* 	      0.625,0}; */
  
  static float s6[]={5, /* SHIP6 TOWER */
		     1.,0,
		     1.,-0.5,
		     1.5,0,
		     1.,0.5,
		     1.,0};

  /*  static float s7[]={14,  SHIP7 PILOT 
		     1.,0,
		     1.,0.25,
		     0.5,0.25,
		     0.5,0.5,
		     0,0.5,
		     0,0.25,
		     -1,0.25,
		     -1,-0.25,
		     0,-0.25,
		     0,-0.5,
		     0.5,-0.5,
		     0.5,-0.25,
		     1,-0.25,
		     1.,0};	      
*/
  static float s7[]={11, /* SHIP7 PILOT */
		     .5,.25,
		     -.25,.25,
		     -.25,-.25,
		     0,-.25,
		     0,-1,
		     .25,-1,
		     .25,-.25,
		     .5,-.25,
		     .5,.75,
		     0,.75,
		     0,.25};	      


  static float s8[]={20, /* SHIP7 PILOT in pod */
		     0.75,0.25,
		     0.75,.75,
		     .25,1.25,
		     -.25,1.25,
		     -.75,.75,
		     -.75,-.75,
		     -.25,-1.25,
		     .25,-1.25,
		     .75,-.75,
		     .75,.25,
		     -.25,.25,
		     -.25,-.25,
		     0,-.25,
		     0,-1,
		     .25,-1,
		     .25,-.25,
		     .5,-.25,
		     .5,.75,
		     0,.75,
		     0,.25};	      


  float *s;
  int i;
  static GdkGC  *gcobj[3];
  
  if(sw==0){
    sw=1;
    gcobj[0]=penRed;
    gcobj[1]=penYellow;
    gcobj[2]=penGreen;
  }

  gheight=GameParametres(GET,GHEIGHT,0);
    
  color=2;
  if(obj->state < 50)color=1;
  if(obj->state < 25)color=0;
  gc2=gcobj[color];

  switch(obj->subtype){
  case SHIP1:
    s=s1;
    break;
  case SHIP2:
    s=s2;
    break;
  case SHIP3:
    s=s3;
    break;
  case SHIP4:
    s=s4;
    break;
  case TOWER:
    s=s6;
    break;
  case PILOT:
    s=s7;
    if(obj->mode!=LANDED)
      s=s8;
    break;

  default:
    s=s1;
    break;
  }
  switch(obj->subtype){
  case PILOT:
    rcosa=obj->radio*cos(obj->a);
    rsina=obj->radio*sin(obj->a);
    
    for(i=0;i<s[0]-1;i++){
      x0=s[2*i+1];
      y0=s[2*i+2];
      x1=s[2*i+3];
      y1=s[2*i+4];
      
      x_0=x0*rcosa-y0*rsina;
      y_0=x0*rsina+y0*rcosa;
      
      x_1=x1*rcosa-y1*rsina;
      y_1=x1*rsina+y1*rcosa;
      
      gdk_draw_line(pixmap,gc,
		    x+x_0,gheight-(y+y_0),
		    x+x_1,gheight-(y+y_1));
      
    }
    gdk_draw_rectangle(pixmap,gc2,TRUE,
		       x-1,gheight-y-1,3,3);

    if(0&&obj->mode!=LANDED){ /* pod */
      
      gdk_draw_arc(pixmap,gc,FALSE,
		 x-1.5*obj->radio,gheight-y-1.5*obj->radio,
		 3*obj->radio,3*obj->radio,0,23040);

    }
    break;

  case SHIP1:
  case SHIP2:
  case SHIP3:
  case SHIP4:
    
    rcosa=obj->radio*cos(obj->a);
    rsina=obj->radio*sin(obj->a);
    
    for(i=0;i<s[0]-1;i++){
      x0=s[2*i+1];
      y0=s[2*i+2];
      x1=s[2*i+3];
      y1=s[2*i+4];
      
      x_0=x0*rcosa-y0*rsina;
      y_0=x0*rsina+y0*rcosa;

      x_1=x1*rcosa-y1*rsina;
      y_1=x1*rsina+y1*rcosa;
      
      gdk_draw_line(pixmap,gc,
		    x+x_0,gheight-(y+y_0),
		    x+x_1,gheight-(y+y_1));
      
    }
    gdk_draw_rectangle(pixmap,gc2,TRUE,
		       x-1,gheight-y-1,3,3);

    /********** engine flares *************/
    if(obj->accel>0){ 
      x0=-.6;//-.04*obj->radio;
      x0=-.04*obj->radio;
      y0=.2;
      x1=x0;
      y1=-.2;
      x2=x0-3*obj->accel/obj->gas_max;
      y2=0;


      x_0=x0*rcosa-y0*rsina;
      y_0=x0*rsina+y0*rcosa;

      x_1=x1*rcosa-y1*rsina;
      y_1=x1*rsina+y1*rcosa;

      x_2=x2*rcosa-y2*rsina;
      y_2=x2*rsina+y2*rcosa;

      gdk_draw_line(pixmap,penCyan,
		    x+x_0,gheight-(y+y_0),
		    x+x_2,gheight-(y+y_2));
      gdk_draw_line(pixmap,penCyan,
		    x+x_1,gheight-(y+y_1),
		    x+x_2,gheight-(y+y_2));
    } 
    /********** --engine flares *************/
    break;

  case SATELLITE:	
    gdk_draw_arc(pixmap,gc,FALSE,
		 x-obj->radio,gheight-y-obj->radio,
		 2*obj->radio,2*obj->radio,0,23040);
    break;

   case TOWER: 

     rcosa=obj->radio*cos(obj->a);
     rsina=obj->radio*sin(obj->a);

     gdk_draw_rectangle(pixmap,gc,FALSE, 
			x-.5*obj->radio,gheight-y-obj->radio, 
			obj->radio,2*obj->radio); 
     for(i=0;i<s[0]-1;i++){
       x0=s[2*i+1];
       y0=s[2*i+2];
       x1=s[2*i+3];
       y1=s[2*i+4];
       
       x_0=x0*rcosa-y0*rsina;
       y_0=x0*rsina+y0*rcosa;
       
       x_1=x1*rcosa-y1*rsina;
       y_1=x1*rsina+y1*rcosa;
       
       gdk_draw_line(pixmap,gc,
		     x+x_0,gheight-(y+y_0),
		     x+x_1,gheight-(y+y_1));
     }
     
     
     gdk_draw_rectangle(pixmap,gc2,TRUE, 
			x-1,gheight-y-1,3,3);   
     break; 
  default:
    fprintf(stderr,"ERROR in DrawShip()\n");
    exit(-1);
    break;
  }
}

void DrawPlanet(GdkPixmap *pixmap,int x,int y, int r){
  /*
    Draw a planet in outer space.

   */
  int i,j;
  int dx,dxl,dy;
  static int desp=0,despr=0;
  float inc;
  float coef;
  float radio2;
  int gheight;
  int rcoef;

  gheight=GameParametres(GET,GHEIGHT,0);

  gdk_draw_arc(pixmap,penBlue,TRUE,x-r,gheight-y-r,
	       2*r,2*r,0,23040);
  
  rcoef=(int)Random(-2);
  Random(r);
  coef=2.0*r;
  radio2=r*r;
  inc=r/10.;
  
  for(i=0;i<200;i++){
    dx=(coef*Random(-1)-r);
    dy=(coef*Random(-1)-r)/10.;/**(10.0*Random(-1)); */
    dxl=dx;
    for(j=0;j<3;j++){
      switch(j){
      case 0:
	despr=.5*desp;
	break;
      case 1:
	despr=.6*desp;
	break;
      case 2:
	despr=.7*desp;
	break;
      default:
	despr=desp;
	break;
      }
      dy+=inc*(2+j);
      
      dx+=3*j*inc+despr;
      dxl+=3*j*inc-despr;
      
      if(dx>=r) 
	dx-=2*r; 
      if(dxl>=r) 
	dxl-=2*r; 
      if(dxl<-r) 
	dxl+=2*r; 
      
      if(dx*dx+dy*dy<radio2){
	gdk_draw_point(pixmap,penWhite,x+dx,gheight-y-(dy));
	/*	    gdk_draw_point(pixmap,penWhite,x+dxl,GameParametres(GET,GHEIGHT,0)-y+(dy)); */
	
      }
      if(dxl*dxl+dy*dy<radio2){
	/*	    gdk_draw_point(pixmap,penWhite,x+dx,GameParametres(GET,GHEIGHT,0)-y-(dy)); */
	gdk_draw_point(pixmap,penWhite,x+dxl,gheight-y+(dy));
      }
    }
  }
  desp++;
  if(desp>2*r)desp=0;

  Random(rcoef);

}

void DrawStars(GdkPixmap *pixmap,int mode,float rx,float ry){
  /*
    fixed stars
   */

  int i;
  float x,y;
  static GdkGC *pen[20];
  static struct Stars stars;
  static int xs[NUMSTARS],ys[NUMSTARS];
  static int color[NUMSTARS];
  static int sw=0;
  static int sw2=0;
  static int numstars;
  int rrelxw,rrelyh;
  static int gwidth=0,gheight=0;


  if(gwidth!=GameParametres(GET,GWIDTH,0)){
    gwidth=GameParametres(GET,GWIDTH,0);
    sw=0;
  }
  if(gheight!=GameParametres(GET,GHEIGHT,0)){
    gheight=GameParametres(GET,GHEIGHT,0);
    sw=0;
  }

  if(sw==0){
    unsigned int col;
    numstars=NUMSTARS;
    if(SLOWMACHINE){
      numstars/=4;
    }

    for(i=0;i<20;i++){
      col=40000*Random(-1)+20000;
      col=20000+i*2000;
      pen[i]=GetPen(NewColor(col,col,col),pixmap);
    }
    for(i=0;i<numstars;i++){
      xs[i]=gwidth*Random(-1);
      ys[i]=gheight*Random(-1);
      color[i]=(int)(20*Random(-1));
    }
    stars.x=xs;
    stars.y=ys;
    stars.color=color;
    sw=1;
  }

  if(mode==RELATIVE){
    rrelxw=(int)(rx)%gwidth;
    rrelyh=(int)(ry)%gheight;
    for(i=0;i<numstars;i++){
      x=stars.x[i]-rrelxw;
      y=stars.y[i]-rrelyh;

      gdk_draw_point(pixmap,pen[stars.color[i]],
		     x+gwidth*((x<0)-(x>gwidth)),
		     gheight*(1-((y<0)-(y>gheight)))-y);
    }
  }
  else{
    for(i=0;i<numstars;i++){
      gdk_draw_point(pixmap,pen[stars.color[i]],stars.x[i],gheight-stars.y[i]);
    }
  }
  sw2++;
  if(!(sw2%4)){ /* the stars twinckle */
    for(i=0;i<numstars/10;i++){
      stars.color[i]++;
      if(stars.color[i]>19){
	stars.color[i]=0;
      }
    }
    sw2=0;
  }
}

void DrawPlanetSurface(GdkPixmap *pixmap,struct Planet *planet,  GdkGC *color){
  /*
    Draw the planet surface
   */  

  Segment *s;
  GdkGC *gc;
  int height,width;
  float sx,sy; /* factor sizes */ 

  height=GameParametres(GET,GHEIGHT,0);
  width=GameParametres(GET,GWIDTH,0);
  sx=(float)width/LXFACTOR;
  sy=(float)height/LYFACTOR;
  /* printf("%f %f %d %d\n",sx,sy,width,height);  */

  s=planet->segment;
  
  while(s!=NULL){
    
    if(s->type==TERRAIN){
      gc=penWhite;
    }
    else{
      gc=color;
    }
    gdk_draw_line(pixmap,gc,
		  sx*s->x0,height-sy*s->y0,
		  sx*s->x1,height-sy*s->y1);

    if(s->type==LANDZONE){
      gdk_draw_line(pixmap,gc,
		    sx*s->x0,height-sy*s->y0+1,
		    sx*s->x1,height-sy*s->y1+1);
      gdk_draw_line(pixmap,gc,
		    sx*s->x0,height-sy*s->y0+2,
		    sx*s->x1,height-sy*s->y1+2);
    }
    s=s->next;
  }
}

int DrawRadar(GdkPixmap *pixmap,Object *obj,struct HeadObjList *lhc,int crash){
  /*
    Draw the ship radar with all near ships.
    returns:
    the number of objects(ships and planets) drawed in radar.
  */

  struct HeadObjList hol; 
  struct ObjList *ls;
  GdkGC *gc;
  int x,y,x0,y0;
  float rx,ry;
  int d;
  int n=0;
  int gheight;


  if(obj==NULL)return(0);

  /* if(obj->in==NULL)n=0; */
  /* else{ */
  /*   n=(obj->in->id); */
  /* } */

  n=0;
  /* check */
  if(n<0||n>GameParametres(GET,GNPLANETS,0)){
    fprintf(stderr,"ERROR in DrawRadar() n=%d\n",n);
    exit(-1);
  }
  
  hol=lhc[n];

  gheight=GameParametres(GET,GHEIGHT,0);
  d=30;

  /*  gc=penWhite; */
  gc=penRed;
  
  x0=GameParametres(GET,GWIDTH,0)/2;
  y0=GameParametres(GET,GHEIGHT,0)/2;
  
  if(!crash){
    gdk_draw_arc(pixmap,gc,FALSE,x0-d,gheight-y0-d,2*d,2*d,0,23040); 
  }
  else{
    gdk_draw_arc(pixmap,penSoftRed,TRUE,x0-d,gheight-y0-d,2*d,2*d,0,23040); 
  }

  
  x=x0;y=y0;
  gdk_draw_point(pixmap,gc,x,gheight-y);

  ls=hol.next;

  while (ls!=NULL){

    if(ls->obj->habitat!=H_SPACE){ls=ls->next;continue;}
    if(ls->obj->type==SHIP && ls->obj->ttl<MINTTL){ls=ls->next;continue;}
    if(ls->obj==obj){ls=ls->next;continue;}

    rx=(ls->obj->x - obj->x);
    ry=(ls->obj->y - obj->y);

    if((rx*rx+ry*ry) > (float)obj->radar*(float)obj->radar){
      ls=ls->next;
      continue;
    }

    x=rx*d/(obj->radar)+x0;
    y=ry*d/(obj->radar)+y0;

    switch(ls->obj->type){
    case SHIP:

      if(ls->obj->player==obj->player){
	gc=penWhite;
      }
      else{
	if(players[ls->obj->player].team==players[obj->player].team){
	  gc=penBlue;
	}
	else{
	  gc=penRed;
	}
      }
      switch(ls->obj->subtype){
      case PILOT:
	{
	  int interval=20;
	  if(ls->obj->life<1200){
	    interval=10;
	  }
	  if(GetTime()%interval < interval/2 ){
	    gdk_draw_line(pixmap,gc,
			  x-1,gheight-y,
			  x+1,gheight-y);
	    gdk_draw_line(pixmap,gc,
			  x,gheight-y-1,
			  x,gheight-y+1);

	  }
	}
	break;
      default:
	gdk_draw_point(pixmap,gc,x,gheight-y);
	gdk_draw_point(pixmap,gc,x+1,gheight-y);
	gdk_draw_point(pixmap,gc,x,gheight-y+1);
	gdk_draw_point(pixmap,gc,x+1,gheight-y+1);
	break;
      }

      n++;
      break;
    case PLANET:
      gc=penBlue;
      gc=gcolors[players[ls->obj->player].color];

      gdk_draw_rectangle(pixmap, 
			 gc, 
			 TRUE, 
			 x-1,gheight-y-1, 
			 3,3);
      n++;
      break;
    case ASTEROID:
      gc=penRed;
      gdk_draw_point(pixmap,gc,x,gheight-y);

      break;
    default:
      ls=ls->next;continue;
      break;
    }
    ls=ls->next;
   }
  return(n);
}


void DrawSpaceShip(GdkPixmap *pixmap,Object *obj,struct HeadObjList *lhc){

  int gwidth,gheight;
  GdkGC *gc;

  float x0,y0,x1,y1,x2,y2;
  int i,j;
  float fx,fy;
  float s[]={8,
	     LINE,-.75,1.5,.75,1.5,
	     LINE,.75,1.5,1.25,1,
	     LINE,1.25,1,1.25,.25,
	     LINE,1.25,.25,.75,-.25,
	     LINE,.75,-.25,-.75,-.25,
	     LINE,-.75,-.25,-1.25,.25,
	     LINE,-1.25,.25,-1.25,1,
	     LINE,-1.25,1,-.75,1.5};


  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);
  gc=penWhite;

  fx=(float)gwidth/4;
  fy=(float)gheight/4;

  x0=(float)gwidth/2;
  y0=(float)gheight/2;
  /*
    x0=2*fx;
    y0=2*fy;
  */
  j=1;
  for(i=0;i<s[0];i++){

    switch((int)s[j]){
    case DOT:
      break;
    case LINE:
      x1=s[++j];
      y1=s[++j];
      x2=s[++j];
      y2=s[++j];
      j++;
      gdk_draw_line(pixmap,gc,
		    x0+x1*fx,gheight-(y0+y1*fy),
		    x0+x2*fx,gheight-(y0+y2*fy));
      break;
    case CIRCLE:
      break;
    default:
      printf("DrawSpaceShip() error\n");
      break;
    }
  }
  return;
}


void DrawAsteroid(GdkPixmap *pixmap,int x,int y,Object *obj){
  /*
    version 01 25Oct10
    Draw an asteroid
  */
  static int sw;
  static float mat_d[4][16];

  GdkGC *gc;
  int i,j;
  float a;
  float factor=12;
  float angfac;
  GdkPoint pts[16];
  int gheight;

  gheight=GameParametres(GET,GHEIGHT,0);
  gc=penWhite;
  
  if(sw==0){
    
    for(j=0;j<4;j++){    
      for(i=0;i<16;i++){
	mat_d[j][i]=1*Random(-1);
      }
    }
  
    sw=1;  /* local */
  }
  angfac=2*PI/16;


  if(obj->subtype==ASTEROID1)
    factor=12;
  if(obj->subtype==ASTEROID2)
    factor=6;
  if(obj->subtype==ASTEROID3)
      factor=3;


  for(i=0;i<16;i++){
    a=i*angfac+obj->a;
    pts[i].x=(mat_d[obj->id%4][i]+2)*factor*cos(a)+x;
    pts[i].y=(mat_d[obj->id%4][i]+2)*factor*sin(a)+gheight-y;
  }

  gdk_draw_polygon(pixmap,gc,
		   FALSE,
		   pts,16);
}



void DrawMap(GdkPixmap *pixmap,int player,struct HeadObjList hol,Object *cv,int ulx){
  /*
    version 02
    Draw an space map with the known universe.

   */  
  struct ObjList *ls;
  struct IntList *ks;

  static struct HeadObjList listheadnearobjs;   /* list of near enemies objects of actual player.*/

  GdkGC *gc;
  int x,y,x0,y0;
  int d,sd;
  int a,b;
  static int label=3;
  float zoom=1;
  float cvx=0,cvy=0;
  static float objx=0,objy=0;
  float factor;
  char point[MAXTEXTLEN];
  int gwidth,gheight;
  int texth;
  int gnet,proc;
  static int createnearobjsw=1;
  static int lasttime;
  static int lastplayer;
  int time;
  float distance;

  time=GetTime();
  if(time>lasttime+10 || time<lasttime || player!=lastplayer){
    createnearobjsw=1;
  }
  
  if(createnearobjsw){
    DestroyObjList(&listheadnearobjs);
    listheadnearobjs.n=0;
    listheadnearobjs.next=NULL;
    CreateNearObjsList(&hol,&listheadnearobjs,player);
    lasttime=GetTime();
    lastplayer=player;
    createnearobjsw=0;
  }


  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);

  if(keys.order.state==FALSE){
    /* DrawString(pixmap,gfont,penRed,10,gheight+GameParametres(GET,GPANEL,0)/2+4,  */
    /* 		    "O: Introduce command");  */
    ShellTitle(0,NULL,pixmap,gfont,penRed,10,gheight+GameParametres(GET,GPANEL,0)/2+4);

    /* map centered at selected ship */
    if(cv!=NULL){
      if(cv->habitat==H_PLANET){
	objx=cv->in->planet->x;
	objy=cv->in->planet->y;
      }
      else{
	objx=cv->x;
	objy=cv->y;
      }
    }

    if(keys.l==TRUE){
      label++;
      if(label>3)
	label=0;
      keys.l=FALSE;
    }
  }
  
  x0=0.5*gwidth;
  y0=0.5*gheight;
  Shift(SET,ulx,cv==NULL?0:cv->id,&zoom,&cvx,&cvy);
  factor=gwidth*zoom/ulx;
  sd=SECTORSIZE*factor;

  /* sectors */
  if(GameParametres(GET,GKPLANETS,0)==FALSE){
    gc=penSoftRed;
    
    if(label&2){
      ks=players[player].ksectors.list;
      
      while(ks!=NULL){
	InvQuadrant(ks->id,&a,&b);
	a*=SECTORSIZE;
	b*=SECTORSIZE;

	x=x0+(a - objx + cvx)*factor;
	if(x<0 || x>gwidth){ks=ks->next;continue;}
	y=y0+(b - objy + cvy)*factor;
	if(y>gheight || y<0 ){ks=ks->next;continue;}

	if(sd>3){
	  gdk_draw_line(pixmap,  
			gc,
			x,gheight-(y),
			x+sd,gheight-(y+sd));
	  
	  gdk_draw_line(pixmap,  
			gc,
			x,gheight-(y+sd),
			x+sd,gheight-(y));
	}
	else{
	  gdk_draw_point(pixmap,  
			 gc,
			 x,gheight-(y));
	}
	
	if(0){
	  if(sd>3){
	    gdk_draw_rectangle(pixmap,  
			       gc,
			       TRUE,  
			       x,gheight-(y),  
			       sd,sd); 
	  }
	  else{
	    if(sd>1){
	      gdk_draw_rectangle(pixmap,  
				 gc,
				 FALSE,  
				 x,gheight-(y),  
				 sd,sd); 
	    }
	    else{
	      gdk_draw_point(pixmap,  
			     gc,
			     x,gheight-(y));
	    }
	  }
	}
	ks=ks->next;
      }
    }
  }
  /* --sectors */

  /***** enemy ships *****/

  gc=penRed;

  gnet=GameParametres(GET,GNET,0);
  proc=GetProc();
  
  ls=listheadnearobjs.next;

  while (ls!=NULL){
    if(ls->obj->habitat!=H_SPACE){ls=ls->next;continue;}
    x=x0+(ls->obj->x-objx+cvx)*factor;
    if(x<0||x>gwidth){ls=ls->next;continue;}

    y=y0+(ls->obj->y-objy+cvy)*factor;
    if(y>gheight || y<0 ){ls=ls->next;continue;}

    switch(ls->obj->type){
    case PLANET:
      break;
    case SHIP:
      if(gnet==TRUE){
	if(proc!=players[ls->obj->player].proc){
	  if(ls->obj->ttl<MINTTL){ls=ls->next;continue;}
	  }
      }
      switch(ls->obj->subtype){
      case PILOT:
	{
	  int interval=20;
	  if(ls->obj->life<1200){
	    interval=10;
	  }
	  if(GetTime()%interval < interval/2 ){
	    gc=penRed;
	    gdk_draw_line(pixmap,gc,
			  x-4,gheight-y,
			  x-2,gheight-y);
	    gdk_draw_line(pixmap,gc,
			  x+2,gheight-y,
			  x+4,gheight-y);
	    gdk_draw_line(pixmap,gc,
			  x,gheight-y-4,
			  x,gheight-y-2);
	    gdk_draw_line(pixmap,gc,
			  x,gheight-y+2,
			  x,gheight-y+4);
	  }
	}
	break;
      default:
	gc=gcolors[players[ls->obj->player].color];
	gdk_draw_line(pixmap,gc,
		      x,gheight-y-2,
		      x,gheight-y+2);
	gdk_draw_line(pixmap,gc,
		      x-2,gheight-y,
		      x+2,gheight-y);
	if(0&&label&1){
	  snprintf(point,MAXTEXTLEN,"%d",ls->obj->pid);
	  DrawString(pixmap,gfont,gc,x+5,gheight-y,point);
	}
      }
      break;
    case ASTEROID:      
      break;

    default:
      break;
    }
    ls=ls->next;
  }
  

  /***** --enemy ships *****/


  /***** planets and ships *****/
  gc=penRed;
  
  ls=hol.next;

  while (ls!=NULL){
    switch(ls->obj->type){
    case SHIP:
      if(ls->obj->habitat!=H_SPACE){ls=ls->next;continue;}
      if(!ENEMIESKNOWN){
	if(players[ls->obj->player].team!=players[player].team){
	  ls=ls->next;continue;
	}
      }
      break;
    case PLANET:
      if(IsInIntList((players[player].kplanets),ls->obj->id)==0){
	ls=ls->next;continue;
      }
      break;
    case ASTEROID:
      break;
    default:
      ls=ls->next;continue;
      break;
    }

    x=x0+(ls->obj->x-objx+cvx)*factor;
    if(x<0||x>gwidth){ls=ls->next;continue;}
    y=y0+(ls->obj->y-objy+cvy)*factor;
    if(y>gheight || y<0 ){ls=ls->next;continue;}

    switch(ls->obj->type){
    case PLANET:
      gc=gcolors[players[ls->obj->player].color];

      gdk_draw_rectangle(pixmap, 
			 gc, 
			 TRUE, 
			 x-1,gheight-y-1,
			 3,3);

      if(label&1){
	if(strcmp(ls->obj->name,"x")!=0)
	  snprintf(point,MAXTEXTLEN,"%d  %s",ls->obj->id,ls->obj->name);
	else{
	  snprintf(point,MAXTEXTLEN,"%d",ls->obj->id);
	}
	/*	DrawString(pixmap,gfont,penRed,x+5,game.height-y,point); */
	DrawString(pixmap,gfont,gc,x+5,gheight-y,point);
      }

      break;
    case SHIP:
      gc=gcolors[players[ls->obj->player].color];
      if(gnet==TRUE){
	if(proc!=players[ls->obj->player].proc){
	  if(ls->obj->ttl<MINTTL){ls=ls->next;continue;}
	}
      }
     switch(ls->obj->subtype){
     case PILOT:
       {
	 int interval=20;
	 if(ls->obj->life<1200){
	   interval=10;
	 }
	 if(GetTime()%interval < interval/2 ){
	   gdk_draw_line(pixmap,gc,
			 x-4,gheight-y,
			 x-2,gheight-y);
	   gdk_draw_line(pixmap,gc,
			 x+2,gheight-y,
			 x+4,gheight-y);
	   gdk_draw_line(pixmap,gc,
			 x,gheight-y-4,
			 x,gheight-y-2);
	   gdk_draw_line(pixmap,gc,
			 x,gheight-y+2,
			 x,gheight-y+4);
	 }
       }
       break;
     default:
       gdk_draw_line(pixmap,gc,
		     x,gheight-y-2,
		     x,gheight-y+2);
       gdk_draw_line(pixmap,gc,
		     x-2,gheight-y,
		     x+2,gheight-y);
       break;
     }
      if(label&1){
	snprintf(point,MAXTEXTLEN,"%d",ls->obj->pid);
	DrawString(pixmap,gfont,gc,x+5,gheight-y,point);
      }
      break;
    case ASTEROID:      

      if(ls->obj->type==ASTEROID){ /* don't draw far asteroids */
	
	if(Distance2NearestShipLessThan(&hol,player,ls->obj->x,ls->obj->y,MAXASTEROIDDISTANCE2)==0){
	  ls=ls->next;continue;
	}
	
	
      }
      gdk_draw_line(pixmap,penWhite,
		    x,gheight-y-2,
		    x,gheight-y+2);
      gdk_draw_line(pixmap,penWhite,
		    x-2,gheight-y,
		    x+2,gheight-y);
      break;

    default:
      break;
    }
    ls=ls->next;
  }
  /***** --planets and ships *****/


  /***** scale *****/

  x0=gwidth-50;
  y0=gheight-20;
  d=70;
  snprintf(point,MAXTEXTLEN,"%.1f",(float)d/(SECTORSIZE*factor));
  DrawString(pixmap,gfont,penGreen,x0+10,y0+5,point);
  gdk_draw_line(pixmap,penGreen,
		x0-d,y0,
		x0,y0);
  gdk_draw_line(pixmap,penGreen,
		x0-d,y0-2,
		x0-d,y0+2);
  gdk_draw_line(pixmap,penGreen,
		x0,y0-2,
		x0,y0+2);
  /***** --scale *****/

  /***** mouse position *****/

  x0=gwidth/2;
  y0=gheight/2;
  
  MousePos(GET,&x,&y);
  Window2Sector(cv,&x,&y);
  
  distance=Distance(cv,x*SECTORSIZE,y*SECTORSIZE);
  
  snprintf(point,MAXTEXTLEN,"%d %d",x,y);
  MousePos(GET,&x,&y);
  gdk_draw_rectangle(pixmap,penBlack,TRUE,x,y-24,30,24);
  DrawString(pixmap,gfont,penGreen,x,y,point);

  texth=gdk_text_height(gfont,"Lp",2);
  snprintf(point,MAXTEXTLEN,"%d",(int)(distance/SECTORSIZE));


  /***** target *****/
  {
    int x,y,a,b;
    int xt,yt;
    Object *targetobj;
    
    MousePos(GET,&x,&y);
    a=x;
    b=y;
    W2R(cv,&a,&b);
    targetobj=ObjNearThan(&hol,player,a,b,500000); /* 0.5 sectors */
    if(targetobj!=NULL){
      snprintf(point,MAXTEXTLEN,"%d %d ",(int)(distance/SECTORSIZE),targetobj->pid);
      xt=x0+(targetobj->x-objx+cvx)*factor;
      yt=y0+(targetobj->y-objy+cvy)*factor;
      gdk_draw_rectangle(pixmap,penRed,FALSE,xt-6,gheight-yt-6,12,12);
    }
    else{
      snprintf(point,MAXTEXTLEN,"%d",(int)(distance/SECTORSIZE));
    }
    //    printf("Window2Real() %d %d %d\n",a,b,pid);
  }
  /***** --target *****/


  DrawString(pixmap,gfont,penGreen,x,y-texth-2,point);

  /***** --mouse position *****/

  }


float Distance(Object *obj,float x,float y){
  float x1,y1,x2,y2;
  float d;

  if(obj==NULL)return(0);

  if(obj->habitat!=H_SPACE){
    x1=obj->in->x;
    y1=obj->in->y;
  }
  else{
    x1=obj->x;
    y1=obj->y;
  }

  x2=x;
  y2=y;
  d=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));

  return(d);

}


int DrawPlayerInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct Player *player,int x0,int y0){
  /*
    Show info about the player
    returns:
    the y vertical position of the last word writed.
   */
  char point[128];
  int y;
  static int n=0;
  static float gold0;
  static float incgold=0;
  static float incgold0=0;
  static int texth=12;
  int incy;

  if(font==NULL)return(y0);

  incgold+=player->gold-gold0;
  gold0=player->gold;
  n++;
  if(n>=20){
    incgold0=incgold;
    incgold=0;
    n=0;
  }
  y=y0;

  texth=gdk_text_height(font,"Lp",2);

  incy=texth;

  sprintf(point,"PLAYER   %d:",player->id); 
  DrawString(pixmap,font,color,x0,y,point); 
  y+=incy; 
  sprintf(point,"GOLD:  %.0f (%.1f) %.1f",player->gold,incgold0,20*player->balance);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;
  sprintf(point,"NPLANETS:  %d",player->nplanets);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;
  sprintf(point,"NSHIPS:  %d",player->nships);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;
  sprintf(point,"Kills:  %d",player->nkills);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;
  sprintf(point,"Deaths:  %d",player->ndeaths);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;
  sprintf(point,"Points: %d",player->points);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;
  return(y);
}

int DrawPlanetInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *planet,int player,int x0,int y0){
  /*
    Show info about the planet
    returns:
    the y vertical position of the last word writed.
   */

  char point[128];
  int y=y0;
  static int texth=12;
  int incy;

  if(font==NULL)return(0);

  
  texth=gdk_text_height(font,"Lp",2);

  incy=texth;

  sprintf(point,"PLANET id: %d",planet->id);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;

  if(planet->player==player){
    sprintf(point,"GOLD: %.0f",planet->planet->gold);
  }
  else{
    sprintf(point,"GOLD: --");
  }

  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;

  if(planet->player==player){
    sprintf(point,"RESOURCES: %.1f",planet->planet->A/0.015);
  }
  else{
    sprintf(point,"RESOURCES: --");
  }
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;

  sprintf(point,"MASS: %d",planet->mass);
  DrawString(pixmap,font,color,x0,y,point);
  y+=incy;

  return(y);

}

int DrawEnemyShipInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *obj,int x0,int y0){
  /*
    Show info about the object obj
    returns:
    the vertical position, y,  of the last word writed.
   */
  
  GdkGC *gc;
  char cad[20];
  int n;
  int x,y;
  int sx=60;
  static int textw=12;
  static int texth=12;
  static int sw=0;
  int incy;

  if(obj==NULL)return(-1);

  if(sw==0){
    if(font!=NULL){
      texth=gdk_text_height(font,"Lp",2);
      textw=gdk_text_width(font,"ENERGY",6);
      sw++;
    }
  }
  x=x0-textw-sx-20;
  y=y0+texth;
  incy=texth+1;

  if(font!=NULL){
    y=y0+texth;

    DrawString(pixmap,font,color,x,y+texth,"ENERGY");
    y+=incy;
    DrawString(pixmap,font,color,x,y+texth,"STATE");
    y+=incy;
    /* production */
    //    sprintf(cad,"%d  L : %d",obj->id,obj->level);
    sprintf(cad,"L : %d",obj->level);
    DrawString(pixmap,font,color,x,y+texth,cad);
    y+=incy;
  }

  gc=penGreen;
  y=y0+texth;
  /*  obj */

  /* energy */
  if(obj->gas_max==0)n=0;
  else{
    n=sx*obj->gas/obj->gas_max;
  }
  if(n>100)n=100;
  if(n<0)n=0;
  if(n>50)gc=penGreen;
  else{  
    if(n<15)gc=penRed;
    else
      gc=penYellow;
  }
  if(obj->type==SHIP&&obj->subtype==PILOT && obj->mode==LANDED)n=0;
  gdk_draw_rectangle(pixmap,gc,TRUE,x+textw+10+1,y+1,n,texth-2);
  gdk_draw_rectangle(pixmap,color,FALSE,x+textw+10,y,sx,texth-1);
  y+=incy;

  /* state */
  n=sx*obj->state/100.;
  gc=penGreen;
  if(n>100)n=100;
  if(n<0)n=0;
  if(n<50)gc=penYellow;
  if(n<25)gc=penRed;
  if(obj->type==SHIP&&obj->subtype==PILOT && obj->mode==LANDED)n=0;
  gdk_draw_rectangle(pixmap,gc,TRUE,x+textw+10+1,y+1,n,texth-2);
  gdk_draw_rectangle(pixmap,color,FALSE,x+textw+10,y,sx,texth-1);
  y+=incy;
  return(y);
}


int DrawShipInfo(GdkPixmap *pixmap,GdkFont *font,GdkGC *color,Object *obj,int x0,int y0){
  /*
    Show info about the ship obj
    returns:
    the y vertical position of the last word writed.
   */

  static GdkGC *gc=NULL;
  GdkGC *wgc;
  struct Order *ord=NULL;
  char point[MAXTEXTLEN];
  char mode=' ';
  int x,y;
  float ox,oy;
  int n;
  float a;
  float v=0;
  float dx,dy,d; /* distance between ship and dest */
  int sx=60;
  static int textw=12;
  static int texth=12;
  int incy;


  d=0;
  gc=color;
  x=x0+10;
  if(obj->ai!=0)mode='A';
  if(obj->ai==0)mode='M';
  if(obj->type==SHIP&&obj->subtype==PILOT)mode='A';
  if(font!=NULL){
    texth=gdk_text_height(font,"Lp",2);
    textw=gdk_text_width(font,"O",1);
  }
  y=y0+texth;
  incy=texth+1;
  
  /* gas */
  if(obj->gas_max==0)n=0;
  else{
    n=sx*obj->gas/obj->gas_max;
  }

  if(n<25)gc=penRed;
  else{
    if(n<50)gc=penYellow;
  }
  if(obj->type==SHIP&&obj->subtype==PILOT && obj->mode==LANDED)n=0;
  gdk_draw_rectangle(pixmap,gc,TRUE,x+1,y+1,n,texth-2);
  gdk_draw_rectangle(pixmap,color,FALSE,x,y,sx,texth-1);
  snprintf(point,MAXTEXTLEN,"ENERGY %.0f",obj->gas);
  DrawString(pixmap,font,color,x+sx+10,y+texth,point);
  //  DrawString(pixmap,font,color,x0+sx+10,y+texth,"ENERGY");	
  y+=incy;

  /* state */
  n=sx*obj->state/100.;
  gc=color;

  if(n<25)gc=penRed;
  else{
    if(n<50)gc=penYellow;
  }
  if(obj->type==SHIP&&obj->subtype==PILOT && obj->mode==LANDED)n=0;
  gdk_draw_rectangle(pixmap,gc,TRUE,x+1,y+1,n,texth-2);
  gdk_draw_rectangle(pixmap,color,FALSE,x,y,sx,texth-1);
  snprintf(point,MAXTEXTLEN,"STATE (Sh:%.1f)",obj->shield);
  DrawString(pixmap,font,color,x+sx+10,y+texth,point);
  y+=incy;
  
  /* experience */
  a=obj->experience/(100*pow(2,obj->level));
  gc=color;
  gdk_draw_rectangle(pixmap,gc,TRUE,x+1,y+1,sx*a,texth-2);
  gdk_draw_rectangle(pixmap,color,FALSE,x,y,sx,texth-1);
  snprintf(point,MAXTEXTLEN,"EXPERIENCE %.0f/%d",obj->experience,(int)(100*pow(2,obj->level)));
  DrawString(pixmap,font,color,x+sx+10,y+texth,point);
  //  DrawString(pixmap,font,color,x0+sx+10,y+texth,"EXPERIENCE");
  y+=incy;


  /* no more graphics */
  if(font==NULL)  return(y+texth);

  //    obj->id=GetNProc()*g_objid+GetProc();    /* identifier */


  snprintf(point,MAXTEXTLEN,"%s id: %d %c  L : %d",TypeCad(obj),obj->pid,mode,obj->level);
  DrawString(pixmap,font,color,x0+x,y+texth,point);
  y+=incy;

  /***** carrying objects *****/

  if(obj->items&ITPILOT){
    Object pilot;
    int gheight;
    gheight=GameParametres(GET,GHEIGHT,0);
    pilot.state=1;
    pilot.type=SHIP;
    pilot.subtype=PILOT;
    pilot.mode=LANDED;
    pilot.radio=texth;
    pilot.a=0;
    pilot.accel=0;
    pilot.gas_max=1;
    DrawShip(pixmap,color,x0+x+pilot.radio,-(y+texth)+gheight,&pilot);
    gdk_draw_rectangle(pixmap,color,FALSE,x0+x,y+texth-texth,2*texth,2*texth);
    y+=2*incy;  
  }


  /* weapon */

  if(obj->weapon==NULL){
    fprintf(stderr,"Warning: weapon NULL in DrawShipInfo()\n");
    obj->weapon=&obj->weapon0;
  }
  snprintf(point,MAXTEXTLEN,"Amm. : %d (D:%d)",obj->weapon->n,obj->weapon->projectile.damage);
  DrawString(pixmap,font,color,x0+x,y+texth,point);
  if(font!=NULL){
    x+=gdk_text_width(font,point,strlen(point))+textw;
  }

  wgc=color;
  if(obj->weapon==&obj->weapon0){
    wgc=penWhite;
  }
  if(obj->weapon0.n==0)wgc=penRed;
  if(obj->weapon0.type==CANNON0)wgc=penRed;
  if(obj->weapon0.cont1)wgc=penRed;
  DrawString(pixmap,font,wgc,x0+x,y+texth,"1");

  x+=1.5*textw;
  wgc=color;
  if(obj->weapon==&obj->weapon1){
    wgc=penWhite;
  }
  if(obj->weapon1.n==0)wgc=penRed;
  if(obj->weapon1.type==CANNON0)wgc=penRed;
  if(obj->weapon1.cont1)wgc=penRed;
  DrawString(pixmap,font,wgc,x0+x,y+texth,"2");

  x+=1.5*textw;
  wgc=color;
  if(obj->weapon==&obj->weapon2){
    wgc=penWhite;
  }
  if(obj->weapon2.n==0)wgc=penRed;
  if(obj->weapon2.type==CANNON0)wgc=penRed;
  if(obj->weapon2.cont1)wgc=penRed;
  DrawString(pixmap,font,wgc,x0+x,y+texth,"3");

  y+=incy;
  x=x0+10;



  /* sector */

  if(obj->habitat==H_PLANET){
    //    printf("ship: %d %d %d %d\n",obj->id,obj->pid,obj->type,obj->subtype);
    ox=obj->in->x;
    oy=obj->in->y;
  }
  else{
    ox=obj->x;
    oy=obj->y;
  }

  v=sqrt(obj->vx*obj->vx + obj->vy*obj->vy);
  
  snprintf(point,MAXTEXTLEN,"Sector: %d   %d",
	  (int)(ox/SECTORSIZE)-(ox<0),
	  (int)(oy/SECTORSIZE)-(oy<0));
  DrawString(pixmap,font,color,x0+x,y+texth,point);
  y+=incy;

  if(0){
    snprintf(point,MAXTEXTLEN,"VX %.1f",obj->vx);
    DrawString(pixmap,font,color,x0+x,y+texth,point);
    y+=incy;
    snprintf(point,MAXTEXTLEN,"VY %.1f",obj->vy);
    DrawString(pixmap,font,color,x0+x,y+texth,point);
    y+=incy;
  }
  snprintf(point,MAXTEXTLEN,"V=%.1f",v);
  DrawString(pixmap,font,color,x0+x,y+texth,point);
  y+=incy;
  if(0){
    snprintf(point,MAXTEXTLEN,"A  %.1f  %f ",180*obj->a/PI,obj->a);
    DrawString(pixmap,font,color,x0+x,y+texth,point);
    y+=incy;
  }

  snprintf(point,MAXTEXTLEN,"kills: %d",obj->kills);
  DrawString(pixmap,font,color,x0+x,y+texth,point);
  y+=incy;

  /* order */
  ord=ReadOrder(NULL,obj,MAINORD);
  if(ord!=NULL){
    /*    strcpy(tmpcad,""); */
    switch(ord->id){
    case GOTO:
      
      if(ord->c!=-1){
	snprintf(point,MAXTEXTLEN,"ORDER GOTO: %d",(int)ord->e);
	dx=ox-ord->a;
	dy=oy-ord->b;
	d=sqrt(dx*dx+dy*dy)/SECTORSIZE;
	
      }
      else{
	snprintf(point,MAXTEXTLEN,"ORDER GOTO: (%d,%d)", 
		 (int)(ord->a/SECTORSIZE)-(ord->a<0),
		 (int)(ord->b/SECTORSIZE)-(ord->b<0));
	dx=ox-ord->a;
	dy=oy-ord->b;
	d=sqrt(dx*dx+dy*dy)/SECTORSIZE;
      }
      break;
    case TAKEOFF:
      snprintf(point,MAXTEXTLEN,"ORDER: TAKE OFF");
      break;
    case NOTHING:
      snprintf(point,MAXTEXTLEN,"ORDER: NOTHING");
      break;
    case STOP:
      snprintf(point,MAXTEXTLEN,"ORDER: STOP");
      break;
    case EXPLORE:
      snprintf(point,MAXTEXTLEN,"ORDER: EXPLORE");
      break;
    case RETREAT:
      snprintf(point,MAXTEXTLEN,"ORDER: RETREAT");
      break;
    default:
      snprintf(point,MAXTEXTLEN,"ORDER id: %d",ord->id);
      break;
      
    }
    /*    snprintf(point,MAXTEXTLEN,"ORDER id: %s",tmpcad); */
    DrawString(pixmap,font,color,x0+x,y+texth,point);
    y+=incy;
  }
  else{
    snprintf(point,MAXTEXTLEN,"ORDER --");
    DrawString(pixmap,font,color,x0+x,y+texth,point);
    y+=incy;
  }

  snprintf(point,MAXTEXTLEN,"Distance: %.1f sectors",d);
  DrawString(pixmap,font,color,x0+x,y+texth,point);
  y+=incy;

  if(v>2){
    snprintf(point,MAXTEXTLEN,"Time: %.0f s.",d*SECTORSIZE/(20*DT*v));
  }
  else{
    snprintf(point,MAXTEXTLEN,"Time: --");
  }
    DrawString(pixmap,font,color,x0+x,y+texth,point);
    y+=incy;

    if(0){
      snprintf(point,MAXTEXTLEN,"NORDER:  %d",obj->norder);
      DrawString(pixmap,font,color,x0+x,y+texth,point);
      y+=incy;
    }
  return(y+texth);
}

int XPrintTextList(GdkPixmap *pixmap,GdkFont *font,char *title,struct TextList *head,int x0,int y0,int width,int height){
  /* 
     print the text list head in pixmap at the position x0, y0.
  */
  struct TextList *lh;
  int x,y,scroll; 
  GdkGC *gc;
  int i,m,n,h;
  static int charw=12;
  static int charh=12;
  int textw0,textw;
  int incy;


  if(font==NULL)return(0);

  charh=gdk_text_height(font,"Lp",2);
  charw=gdk_text_width(font,"O",1);
  
  incy=charh+2;

  h=height/incy;

  n=CountTextList(head);
  m=PosTextList(head,1);
  scroll=0;
  if(m>h/2 &&n>h){
    scroll=m-h/2;
  }
  gc=penGreen;

  x=x0+7;
  y=y0+incy;

  textw=0;
  lh=head->next;

  DrawString(pixmap,font,penBlue,x,y,title);
  textw0=gdk_text_width(font,title,strlen(title));
  if(textw0>textw){
    textw=textw0;
  }

  //  y+=incy;

  for(i=0;i<n;i++){ /* HERE double loop */
    switch(lh->color){
    case 1:
      gc=penWhite;
      break;
    case 2:
      gc=penYellow;
      break;
    case 3:
      gc=penRed;
      break;
    case 4:
      gc=penBlue;
      break;
    default:
      gc=penGreen;
      break;
    }

    if(y-scroll*incy>y0){
      DrawString(pixmap,font,gc,x,y+incy-scroll*incy,lh->text);
      //      printf("XPrintTextList(): %s %d\n",lh->text,GetTime());
      textw0=gdk_text_width(font,lh->text,strlen(lh->text));
      if(textw0>textw){
	textw=textw0;
      }
    }
    else{
      i--;
    }
    lh=lh->next;
    y+=incy;
    if(y-scroll*incy>h*incy+y0-5)break;
    if(lh==NULL)lh=head->next;
  }
  gdk_draw_rectangle(pixmap,penGreen,FALSE,
		     x0,y0,textw+charw+10,height);

  return(0);
}


int XPrintMenuHead(GdkPixmap *pixmap,GdkFont *font,struct MenuHead *head,int x0,int y0){
  /* 
     print the text list head in pixmap at the position x0, y0.
  */
  struct MenuItem *item;
  int x,y,scroll,w; 
  GdkGC *gc;
  static int charw=12;
  static int charh=12;
  static int cont=0;
  int incy;
  char point[MAXTEXTLEN];
  char point2[MAXTEXTLEN];
  char text[MAXTEXTLEN];

  if(font==NULL)return(0);
  if(head==NULL)return(0);


  cont++;
  if(cont>20)cont=0;
  strcpy(point2,"");

  charh=gdk_text_height(font,"Lp",2);
  charw=gdk_text_width(font,"O",1);

  incy=charh+2;
  gc=penGreen;

  x=x0+7;
  y=y0+incy;
  scroll=1;

  DrawString(pixmap,font,gc,x,y-scroll*incy,head->title);
  y+=2*incy;

  item=head->firstitem;
  while(item!=NULL){
    //    printf("%d %d: \"%s\"\n",x,y-scroll*incy,item->text);
    if(item->active)gc=penWhite;
    else gc=penGreen;
    switch(item->type){
    case MENUITEMACTION:
      snprintf(point,MAXTEXTLEN,"%s ",item->text);
      break;
    case MENUITEMBOOL:
    case MENUITEMTEXT:

      snprintf(point,MAXTEXTLEN,"%s %s",item->text,GetOptionValue(item->id));
      break;
    case MENUITEMTEXTENTRY:
      if(item->active<ITEM_ST_EDIT){
	snprintf(point,MAXTEXTLEN,"%s %s",item->text,GetOptionValue(item->id));
      }
      else{
	snprintf(point,MAXTEXTLEN,"%s ",item->text);
	if(cont%2){
	  snprintf(point2,MAXTEXTLEN,"%s",GetTextEntry(item,text));
	}
	else{
	  snprintf(point2,MAXTEXTLEN,"%s_",GetTextEntry(item,text));
	}
      }
      break;
    case MENUITEMGRABKEY:
      if(item->active<ITEM_ST_EDIT){
	snprintf(point,MAXTEXTLEN,"%s %s",item->text,GetOptionValue(item->id));
      }
      else{
	snprintf(point,MAXTEXTLEN,"%s ",item->text);
	if(cont%2){
	  snprintf(point2,MAXTEXTLEN,"%s",GetTextEntry(item,text));
	}
	else{
	  snprintf(point2,MAXTEXTLEN,"%s_",GetTextEntry(item,text));
	}
      }
      break;
    default:
      fprintf(stderr,"ERROR XPrintMenuHead(): type %d not implemented\n",item->type);
      break;
    }
    DrawString(pixmap,font,gc,x,y-scroll*incy,point);
    if(item->active==ITEM_ST_EDIT){
      if(strlen(point2)>0){
	gc=penRed;
	w=strlen(point)*charw;
	DrawString(pixmap,font,gc,x+w,y-scroll*incy,point2);
      }
    }
    item=item->next;
    y+=incy;
  }
  return(0);
}



gint Load(GtkWidget *widget,gpointer gdata){

  keys.load=TRUE;
  if(GameParametres(GET,GMODE,0)==CLIENT)
    keys.load=FALSE;
  return(0);
}
 
gint Save(GtkWidget *widget,gpointer gdata){
  keys.save=TRUE;
  if(GameParametres(GET,GMODE,0)==CLIENT)
    keys.save=FALSE;
  return(0);
}

gint Options(GtkWidget *widget,gpointer gdata){
  return(0);
}

int CountKey(int mode){
  static int n_keystrokes=0;
  if(mode==0)
    n_keystrokes=0;
  if(mode==1)
    n_keystrokes++;

  return(n_keystrokes);
}

GdkFont *InitFonts(char *fname){
  /*
    Try to found an available font in the system

   */
  int i;
  char fontname[128];
  GdkFont *font=NULL;

/* command line font type */

  strncpy(fontname,fname,128);
  font=gdk_font_load(fontname);

  /* default font type */
  if(font==NULL){
    fprintf(stderr,"WARNING: font %s not found. Using default.\n",fontname);
    for(i=0;i<NUMFONTNAMES;i++){
      strncpy(fontname,fontnames[i],128);
      font=gdk_font_load(fontname);
      if(font!=NULL){
	break;
      }
      else{
	printf("font %s not found\n",fontname); 
      }
    }
    if(font==NULL){
      fprintf(stderr,"Error: no fonts found in your system\n");
      exit(-1);
    }
  }
  //  printf("using font: %s\n",fontname);
  printf("using font: ");
  PrintFontName(fontname,1);
  printf("\n");
  return(font);
}

GdkFont *InitFontsMenu(char *fname){
  /*
    Try to found an available font in the system

   */
  int i;
  char fontname[128];
  GdkFont *font=NULL;

/* command line font type */

  strncpy(fontname,fname,128);
  font=gdk_font_load(fontname);

  /* default font type */

  if(font==NULL){
    for(i=0;i<NUMFONTNAMES;i++){
      strncpy(fontname,fontnames2[i],128);
      font=gdk_font_load(fontname);
      if(font!=NULL){
	break;
      }
      else{
	printf("font %s not found\n",fontname); 
      }
    }
    if(font==NULL){
      fprintf(stderr,"Error: no fonts found in your system\n");
      exit(-1);
    }
  }
  printf("Menu using font: %s\n",fontname);

  return(font);
}



/* options window */

GtkWidget *CreateSubMenu(GtkWidget *menu,char *szName){

  GtkWidget *menuitem;
  GtkWidget *submenu;

  menuitem=gtk_menu_item_new_with_label(szName);
  gtk_menu_append(GTK_MENU(menu),menuitem);
  gtk_widget_show(menuitem);
  submenu=gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);
 
 return(submenu);

}


gint SetDefaultOptions(GtkWidget *widget,gpointer gdata){
  /* only show in the window the default options
     doesn't make changes
     changes are made when save button is pressed.
  */
  gboolean state;
  const gchar *text;
  char cad[128];
  
  g_print("%s\n",(char *) gdata);

  gtk_toggle_button_set_active((GtkToggleButton *)options1,FALSE);
  state=gtk_toggle_button_get_active((GtkToggleButton *)options1);
  printf("\tstate Known Universe: %d\n",state);

  gtk_toggle_button_set_active((GtkToggleButton *)options2,FALSE);
  state=gtk_toggle_button_get_active((GtkToggleButton *)options2);
  printf("\tstate music: %d\n",state);

  gtk_toggle_button_set_active((GtkToggleButton *)options3,FALSE);
  state=gtk_toggle_button_get_active((GtkToggleButton *)options3);
  printf("\tstate sound: %d\n",state);

  snprintf(cad,MAXTEXTLEN,"%d",NUMPLANETS);
  gtk_entry_set_text((GtkEntry *)options5,cad);
  text=gtk_entry_get_text((GtkEntry *)options5);
  printf("\tplanets: %s\n",text);

  snprintf(cad,MAXTEXTLEN,"%d",NUMPLAYERS);
  gtk_entry_set_text((GtkEntry *)options7,cad);
  text=gtk_entry_get_text((GtkEntry *)options7);
  printf("\tplayers: %s\n",text);

  snprintf(cad,MAXTEXTLEN,"%d",ULX);
  gtk_entry_set_text((GtkEntry *)options9,cad);
  text=gtk_entry_get_text((GtkEntry *)options9);
  printf("\tUniverse size: %s\n",text);

  gtk_toggle_button_set_active((GtkToggleButton *)options14,FALSE);
  state=gtk_toggle_button_get_active((GtkToggleButton *)options14);
  printf("\tstate cooperative: %d\n",state);

  gtk_toggle_button_set_active((GtkToggleButton *)options15,FALSE);
  state=gtk_toggle_button_get_active((GtkToggleButton *)options15);
  printf("\tstate computer cooperative: %d\n",state);

  gtk_toggle_button_set_active((GtkToggleButton *)options16,FALSE);
  state=gtk_toggle_button_get_active((GtkToggleButton *)options16);
  printf("\tstate Queen mode: %d\n",state);

  return(0);
}

gint PrintMessage(GtkWidget *widget,gpointer gdata){
  /* production */
  return(0);
  g_print("%s\n",(char *) gdata);
  return 0;
}

gint ShowWindowOptions(GtkWidget *widget,gpointer gdata){
  /*
    version 01 12May11
  */
  int value;
  char cad[MAXTEXTLEN];
  int show=0;
#if DEBUG
  gboolean state;
  const gchar *text;
#endif

#if DEBUG
  if(debugoptions)g_print("Reading options from file: %s\n",(char *) gdata);
#endif

  if(GameParametres(GET,GNET,0)==TRUE){
    if(GameParametres(GET,GMODE,0)==TRUE){ 
      show=1; // HERE what show 
    }   
  }

  show=1;

  if(show==1){
    value=GameParametres(GET,GKPLANETS,0);
    gtk_toggle_button_set_active((GtkToggleButton *)options1,value);

#if DEBUG
    state=gtk_toggle_button_get_active((GtkToggleButton *)options1);
    if(debugoptions)printf("\tstate Known Universe: %d\n",state);
#endif    

    value=GameParametres(GET,GMUSIC,0);
    if(value>0){
      gtk_toggle_button_set_active((GtkToggleButton *)options2,FALSE);
    }
    else{
      gtk_toggle_button_set_active((GtkToggleButton *)options2,TRUE);
    }
#if DEBUG
    state=gtk_toggle_button_get_active((GtkToggleButton *)options2);
    if(debugoptions)printf("\tstate music: %d vol: %d\n",state,value);
#endif    

    value=GameParametres(GET,GSOUND,0);
    if(value>0){
      gtk_toggle_button_set_active((GtkToggleButton *)options3,FALSE);
    }
    else{
      gtk_toggle_button_set_active((GtkToggleButton *)options3,TRUE);
      }
#if DEBUG
    state=gtk_toggle_button_get_active((GtkToggleButton *)options3);
    if(debugoptions)  printf("\tstate sound: %d vol: %d\n",state,value);
#endif    

    value=GameParametres(GET,GNPLANETS,0);
    snprintf(cad,MAXTEXTLEN,"%d",value);
    gtk_entry_set_text((GtkEntry *)options5,cad);
#if DEBUG
    text=gtk_entry_get_text((GtkEntry *)options5);
    if(debugoptions)  printf("\tplanets: %s\n",text);
#endif    

    value=GameParametres(GET,GNPLAYERS,0);
    snprintf(cad,MAXTEXTLEN,"%d",value);
    gtk_entry_set_text((GtkEntry *)options7,cad);

#if DEBUG
    text=gtk_entry_get_text((GtkEntry *)options7);
    if(debugoptions)  printf("\tplayers: %s\n",text);
#endif    

    value=GameParametres(GET,GULX,0);
    snprintf(cad,MAXTEXTLEN,"%d",value);
    gtk_entry_set_text((GtkEntry *)options9,cad);

#if DEBUG
    text=gtk_entry_get_text((GtkEntry *)options9);
    if(debugoptions)  printf("\tUniverse size: %s\n",text);
#endif    
    
    value=GameParametres(GET,GCOOPERATIVE,0);
    gtk_toggle_button_set_active((GtkToggleButton *)options14,value);

#if DEBUG
    state=gtk_toggle_button_get_active((GtkToggleButton *)options14);
    if(debugoptions) printf("\tCooperative mode: %d\n",state);
#endif    

    value=GameParametres(GET,GCOMPCOOPERATIVE,0);
    gtk_toggle_button_set_active((GtkToggleButton *)options15,value);

#if DEBUG
    state=gtk_toggle_button_get_active((GtkToggleButton *)options15);
    if(debugoptions)  printf("\tComputer cooperative mode: %d\n",state);
#endif    

    value=GameParametres(GET,GQUEEN,0);
    gtk_toggle_button_set_active((GtkToggleButton *)options16,value);

#if DEBUG
    state=gtk_toggle_button_get_active((GtkToggleButton *)options16);
    if(debugoptions)  printf("\tQueen mode: %d\n",state);
#endif
    }

  /* show window */
  gtk_widget_show(winoptions);
  return 0;
}


gint SaveWindowOptions(GtkWidget *widget,gpointer gdata){
  gboolean state,status;
  const gchar *text;
  FILE *fp;
  int value;
  int nplayers;
  int nplanets;


#if DEBUG
  if(debugoptions){
    g_print("GDATA:%s\n",(char *) gdata);
    printf("configuration file: %s\n",(char *) gdata);
  }
#endif

  if((fp=fopen((char *) gdata,"wt"))==NULL){
    fprintf(stdout,"SaveWindowOptions(): Cant open the file: %s",(char *) gdata);
    exit(-1);
  }

  state=gtk_toggle_button_get_active((GtkToggleButton *)options1);
  if((int)state!=FALSE && (int)state!=TRUE)state=FALSE;
#if DEBUG
  if(debugoptions)printf("\tstate Known Universe: %d\n",state);
#endif  
  param.kplanets=state;
  //  fprintf(fp,"%d ",(int)state);

  state=gtk_toggle_button_get_active((GtkToggleButton *)options2);
  if((int)state!=FALSE && (int)state!=TRUE)state=TRUE;
#if DEBUG
  if(debugoptions)printf("\tstate music: %d\n",state);
#endif
  GameParametres(SET,GMUSIC,!state);
  param.music=!state;
  //  fprintf(fp,"%d ",(int)state);
  if(state){
    SetMusicVolume(0,VOLSET);
  }
  else{
    SetMusicVolume((float)param.musicvol/100,VOLSET);
  }

  state=gtk_toggle_button_get_active((GtkToggleButton *)options3);
  if((int)state!=FALSE && (int)state!=TRUE)state=FALSE;
#if DEBUG
  if(debugoptions)printf("\tstate sound: %d\n",state);
#endif
  GameParametres(SET,GSOUND,!state);
  param.sound=!state;

  if(state){
    SetMusicVolume(0,VOLSET);
    SetSoundVolume(0,VOLSET);
  }
  else{
    SetSoundVolume((float)param.soundvol/100,VOLSET);
  }


  //  fprintf(fp,"%d ",(int)state);

   if(GameParametres(GET,GSOUND,0)==TRUE && GameParametres(GET,GMUSIC,0)==TRUE){
     status=Sound(SPLAY,MUSIC);
#if DEBUG
     if(debugoptions)printf("play music %d\n",status);
#endif
   }
   else{
     status=Sound(SSTOP,MUSIC);
#if DEBUG
     if(debugoptions)printf("stop music %d\n",status);
#endif
   }  

   text=gtk_entry_get_text((GtkEntry *)options5); 
   value=atol(text);
   if(value<MINNUMPLANETS)value=MINNUMPLANETS;
   if(value>MAXNUMPLANETS)value=MAXNUMPLANETS;
   nplanets=value;

   text=gtk_entry_get_text((GtkEntry *)options7); 
   nplayers=atol(text);

   if(nplayers<MINNUMPLAYERS)nplayers=MINNUMPLAYERS;
   if(nplayers>MAXNUMPLAYERS)nplayers=MAXNUMPLAYERS;
   if(nplanets<nplayers)nplanets=nplayers;
#if DEBUG
   if(debugoptions)printf("\tplanets: %d\n",nplanets); 
#endif
   param.nplanets=nplanets;
   //   fprintf(fp,"%d ",nplanets); 
#if DEBUG
   if(debugoptions)printf("\tplayers: %d\n",nplayers); 
#endif
   param.nplayers=nplayers;
   //   fprintf(fp,"%d ",nplayers); 

   text=gtk_entry_get_text((GtkEntry *)options9); 
   value=atol(text);
   if(value<MINULX)value=MINULX;
   if(value>MAXULX)value=MAXULX;
#if DEBUG
   if(debugoptions)printf("\tUniverse size: %d\n",value); 
#endif
   param.ul=value;
   //   fprintf(fp,"%d ",value); 

   state=gtk_toggle_button_get_active((GtkToggleButton *)options14);
   if((int)state!=FALSE && (int)state!=TRUE)state=FALSE;
#if DEBUG
   if(debugoptions)printf("\tstate cooperative: %d\n",state);
#endif
   param.cooperative=(int)state;
   //   fprintf(fp,"%d ",(int)state);

   state=gtk_toggle_button_get_active((GtkToggleButton *)options15);
   if((int)state!=FALSE && (int)state!=TRUE)state=FALSE;
#if DEBUG
   if(debugoptions)printf("\tstate computer cooperative: %d\n",state);
#endif
   param.compcooperative=(int)state;
   //   fprintf(fp,"%d ",(int)state);

   state=gtk_toggle_button_get_active((GtkToggleButton *)options16);
   if((int)state!=FALSE && (int)state!=TRUE)state=FALSE;
#if DEBUG
   if(debugoptions)printf("\tstate Queen mode: %d\n",state);
#endif
   param.queen=(int)state;
   //   fprintf(fp,"%d ",(int)state);

   //   fprintf(fp,"\n");
   fclose(fp);
   SaveParamOptions((char *) gdata,&param);

   gtk_widget_hide(winoptions);
   return 0;
 }


gint ShowWindow(GtkWidget *widget,gpointer gdata){
  
  /* show window */
  gtk_widget_show((GtkWidget *)gdata);
  return TRUE;
}


gint QuitWindow(GtkWidget *widget,gpointer gdata){
  printf("QUIT 1 window\n");
  gtk_widget_hide((GtkWidget *)gdata);
  printf("QUIT 2 window\n");
  return TRUE;
}


GtkWidget *CreateMenuItem(GtkWidget *menu,
			  char *Name,char *Accel,char *Tip,
			  GtkSignalFunc func,
			  gpointer data){
  
  GtkWidget *menuitem;
  if(Name && strlen(Name)){
    menuitem=gtk_menu_item_new_with_label(Name);
    /*    g_print("CMI menu: %s  arg: %s  \n",Name,(char *)data); */
    gtk_signal_connect(GTK_OBJECT (menuitem),"activate",
		       GTK_SIGNAL_FUNC(func),data);
  }else{
    menuitem=gtk_menu_item_new();
  }
  
  gtk_menu_append(GTK_MENU(menu),menuitem);
  gtk_widget_show(menuitem);
  
  
  return(menuitem);
  
}

GtkWidget *CreateBarSubMenu(GtkWidget *menu,char *szName){
  
  GtkWidget *menuitem;
  GtkWidget *submenu;
  
  menuitem=gtk_menu_item_new_with_label(szName);
  gtk_menu_bar_append(GTK_MENU_BAR(menu),menuitem);
  gtk_widget_show(menuitem);
  submenu=gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem),submenu);
  
  return(submenu);
  
}

void SetDefaultKeyValues(struct Keys *key,int action){
  int i;
  
  key->load=key->save=FALSE;
  key->up=key->down=key->right=key->left=key->back=FALSE;
  key->centermap=key->trace=key->tab=key->enter=FALSE;
  key->s=key->n=key->l=FALSE;
  key->e=key->y=key->u=FALSE;
  key->f1=key->f2=key->f3=key->f4=key->f7=key->f8=key->f9=key->f10=FALSE;
  key->p=FALSE;
  key->d=FALSE;
  key->home=key->Pagedown=key->Pageup=key->may=key->ctrl=FALSE;
  for(i=0;i<10;i++)key->number[i]=FALSE;
  key->plus=key->minus=FALSE;

  key->fire.state=FALSE;
  key->turnleft.state=FALSE;
  key->turnright.state=FALSE;
  key->accel.state=FALSE;
  key->automode.state=FALSE;
  key->manualmode.state=FALSE;
  key->order.state=FALSE;
  key->map.state=FALSE;

  /* don't reset this values when load a game */
  switch(action){
  case 1:
    key->map.state=FALSE;
    key->f5=key->f6=FALSE;
    break;
  default:
    break;
  }
}


void DrawPlayerList(GdkPixmap *pixmap,int player,struct HeadObjList *hlp,Object *cvobj,int act){
  /*
    Build and draw two text list:  
    A list with player ships and a list with player planets. 
   */

  struct ObjList *ls;
  Object *obj;
  char cad[MAXTEXTLEN]="";
  char tmpcad[MAXTEXTLEN]="";
  char titleships[MAXTEXTLEN]="";
  static int sw=0;
  char mode=' ';
  struct Order *ord=NULL;
  int pid;
  static struct TextList shiplist;
  static struct TextList planetlist;
  static Object *last_cv=NULL;
  static int textpw=1,textsw=1;
  static int charw=12;
  static int nt,ne,nf,nq;
  static int shipcounter=0;
  int color; /* 0 green, 1 white, 2 yellow, 3 red */
  int textw=0;
  
  if(gfont==NULL)return;

  if(sw==0){ /* variable initialization  */
    shiplist.next=NULL;
    planetlist.next=NULL;
    charw=gdk_text_width(gfont,"O",1);
    shipcounter=0;
    nt=ne=nf=nq=0;
    sw=1;
  }
  shipcounter++;
  if(shipcounter==20){
    ne=CountObjs(hlp,player,SHIP,EXPLORER);
    nf=CountObjs(hlp,player,SHIP,FIGHTER); 
    nt=CountObjs(hlp,player,SHIP,TOWER); 
    nq=CountObjs(hlp,player,SHIP,QUEEN); 
    shipcounter=0;
  }
  snprintf(titleships,MAXTEXTLEN,"SHIPS: E:%d F:%d T:%d Q:%d",ne,nf,nt,nq);

  if(cvobj!=last_cv || act){
    last_cv=cvobj;
    
    DestroyTextList(&shiplist);
    DestroyTextList(&planetlist);

    /* Add2TextList(&shiplist,"SHIPS:",0); */
    //    Add2TextList(&shiplist,tmpcad,0);
    //Add2TextList(&planetlist,"PLANETS:",0);

    textpw=gdk_text_width(gfont,"PLANETS:",strlen("PLANETS:"));
    textsw=gdk_text_width(gfont,"SHIPS:",strlen("SHIPS:"));

    textw=gdk_text_width(gfont,cad,strlen(cad));
    if(textw>textpw)textpw=textw;


    color=0;
    if(cvobj!=NULL){
      if(cvobj->habitat==H_SPACE)color=1;
    }
    snprintf(cad,MAXTEXTLEN,"out: %d",
	     CountShipsInPlanet(hlp,0,SHIP,-1,0));
    textw=gdk_text_width(gfont,cad,strlen(cad));
    if(textw>textpw)textpw=textw;
    Add2TextList(&planetlist,cad,color);
    

    
    ls=hlp->next;
    while(ls!=NULL){
      obj=ls->obj;
      color=0;
      if(obj->type==SHIP){
	mode=Type(obj);
	if(obj->ai==0)mode='M';
	if(obj->type==SHIP && obj->subtype==PILOT)mode='A';	

	if(obj->state<50 || obj->gas<0.5*obj->gas_max){color=2;}
	if(obj->state<25 || obj->gas<0.25*obj->gas_max){color=3;}
	
	if(obj==cvobj){color=1;}
	if(obj->selected==TRUE){color=1;}

	snprintf(cad,MAXTEXTLEN,"%c L%d id: %d ",mode,obj->level,obj->pid);
	
	if(obj->state<100){
	  strncpy(tmpcad,cad,MAXTEXTLEN);
	  snprintf(cad,MAXTEXTLEN,"%s s:%.0f",tmpcad,obj->state);
	}
	if(obj->gas<obj->gas_max){
	  strncpy(tmpcad,cad,MAXTEXTLEN);
	  snprintf(cad,MAXTEXTLEN,"%s e:%d",tmpcad,(int)(100*obj->gas/obj->gas_max));
	}
	ord=ReadOrder(NULL,obj,MAINORD);
	if(ord!=NULL){
	  strncpy(tmpcad,cad,MAXTEXTLEN);
	  switch(ord->id){
   
	  case GOTO:
	    if(ord->c!=-1){
	      snprintf(cad,MAXTEXTLEN,"%s GT:%d",tmpcad,(int)ord->e);
	    }
	    else{
	      strncpy(tmpcad,cad,MAXTEXTLEN);
	      snprintf(cad,MAXTEXTLEN,"%s GT:(%d,%d)",
		       tmpcad,
		       (int)(ord->a/SECTORSIZE)-(ord->a<0),
		       (int)(ord->b/SECTORSIZE)-(ord->b<0));
	    }
	    break;
	  case EXPLORE:
	    snprintf(cad,MAXTEXTLEN,"%s EXP.",tmpcad);
	    break;
	    
	  default:
	    break;
	  }
	}
	strncpy(tmpcad,cad,MAXTEXTLEN);
	pid=0;
	if(obj->in!=NULL){
	  pid=obj->in->pid;
	  if(obj->in->player!=obj->player){
	    snprintf(cad,MAXTEXTLEN,"%s IN:(%d)%d",tmpcad,obj->in->player,pid);
	  }
	  else{
	    snprintf(cad,MAXTEXTLEN,"%s IN:%d",tmpcad,pid);
	  }
	}
	else{
	  snprintf(cad,MAXTEXTLEN,"%s IN:%d",tmpcad,pid);
	}
	textw=gdk_text_width(gfont,cad,strlen(cad));
	if(textw>textsw)textsw=textw;
	Add2TextList(&shiplist,cad,color);
      }/*ship */
      
      if(obj->type==PLANET){
	color=0;

	if(cvobj!=NULL){
	  if(cvobj->player!=obj->player)color=4;
	  if(cvobj->in==obj)color=1;
	}

	snprintf(cad,MAXTEXTLEN,"id: %d g:%.0f  %d",
		 obj->id,obj->planet->gold,CountShipsInPlanet(hlp,obj->id,SHIP,-1,0));
	textw=gdk_text_width(gfont,cad,strlen(cad));
	if(textw>textpw)textpw=textw;
	Add2TextList(&planetlist,cad,color);
      }
      else{
      }
      ls=ls->next;
    }
  }
 
   XPrintTextList(pixmap,gfont,titleships,&shiplist,10,15,textsw+charw+10,GameParametres(GET,GHEIGHT,0)-50); 

  XPrintTextList(pixmap,gfont,"PLANETS:",&planetlist,GameParametres(GET,GWIDTH,0)-textpw-20,15,textpw+charw+10,GameParametres(GET,GHEIGHT,0)-50);

  return;
}

void Shift(int action,int ulx,int objid,float *z,float *x,float *y){
  static float cvx=0,cvy=0;
  static float zoom=1;
  static int id0=0;
  int id;

  if(objid<0)objid=0;

  id=objid;

  if(id0!=id){
    if(MAPAUTOCENTER)cvx=cvy=0;
    id0=id;
  }

  switch(action){
  case GET:
    break;
  case SET:
    if(keys.z==TRUE){
      if(keys.may==FALSE){
	zoom/=1.05;
	if(zoom<.1)zoom=.1;
      }
      else{
	zoom*=1.05;
	if(zoom>64)zoom=64;
      }
    }
    
    if(keys.centermap==TRUE && keys.order.state==FALSE){
      cvx=cvy=0;
    }
    
    if(keys.right==TRUE){
      cvx-=.02*ulx/zoom;
    }
    if(keys.left==TRUE){
      cvx+=.02*ulx/zoom;
    }
    if(keys.up==TRUE){
      cvy-=.02*ulx/zoom;
    }
    if(keys.down==TRUE){
      cvy+=.02*ulx/zoom;
    }
    break;
  default:
    break;
  }
  
  *z=zoom;
  *x=cvx;
  *y=cvy;

}


void Window2Real(Object *cv,int habitat, int wx,int wy,int *rx,int *ry){
  /* version 01 */
  float zoom;
  float cvx,cvy;
  int objx=0,objy=0;
  int ulx;
  int gwidth,gheight;
  int x,y,x0,y0;
  float ifactor;
  float sx,sy;


  ulx=GameParametres(GET,GULX,0);
  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);

  if(cv!=NULL){
    if(cv->habitat==H_PLANET){
      objx=cv->in->planet->x;
      objy=cv->in->planet->y;
    }
    else{
      objx=cv->x;
      objy=cv->y;
    }
  }
  if(habitat==0){  /* free space */
    Shift(GET,ulx,cv==NULL?0:cv->id,&zoom,&cvx,&cvy);
    
    x0=0.5*gwidth;
    y0=0.5*gheight;
    
    x=wx;
    y=gheight-wy;
    
    ifactor=ulx/(gwidth*(float)zoom);
    
    *rx=(x-x0)*ifactor-cvx+objx;
    *ry=(y-y0)*ifactor-cvy+objy;
  }
  else{
    sx=(float)gwidth/LXFACTOR;
    sy=(float)gheight/LYFACTOR;
    *rx=((float)wx/sx+0.5);
    *ry=((float)wy/sy+0.5);
  }
}


void Real2Window(Object *cv,int habitat,int rx,int ry,int *wx,int *wy){
  /* version 01 */

  float zoom;
  float cvx,cvy;
  int objx=0,objy=0;
  int ulx;
  int gwidth,gheight;
  float x0,y0;
  float factor;
  float sx,sy;

  ulx=GameParametres(GET,GULX,0);
  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);


  if(cv!=NULL){
    if(cv->habitat==H_PLANET){
      objx=cv->in->planet->x;
      objy=cv->in->planet->y;
    }
    else{
      objx=cv->x;
      objy=cv->y;
    }
  }
  if(habitat==0){ /* free space */
    Shift(GET,ulx,cv==NULL?0:cv->id,&zoom,&cvx,&cvy);
    
    x0=0.5*gwidth;
    y0=0.5*gheight;
    
    factor=gwidth*(float)zoom/ulx;
    
    *wx=x0+(rx-objx+cvx)*factor + 0.5;
    *wy=gheight-y0-(ry-objy+cvy)*factor + 0.5;
  }
  else{

    sx=(float)gwidth/LXFACTOR;
    sy=(float)gheight/LYFACTOR;
    *wx=(float)rx*sx + 0.5;
    *wy=(float)ry*sy + 0.5;
  }
}

void Window2Sector(Object *cv,int *x,int *y){
  /*
    convert the window x,y coordinates in sectors. 
  */

  int a,b;

  if(cv==NULL)return;
  a=*x;
  b=*y;
  //  printf("Window2Sector() %d %d\n",a,b);
  W2R(cv,&a,&b);
  // printf("\tWindow2Sector() %d %d\n",a,b);
  Real2Sector(a,b,x,y);
}


void W2R(Object *obj,int *x,int *y){
  /*
    convert the window x,y coordinates in real coordinates. 
  */

  int a,b;
  float ifactor;
  int gwidth,gheight,ulx;
  float zoom=1;
  float cvx,cvy;
  float objx,objy;

  if(obj==NULL)return;

  gheight=GameParametres(GET,GHEIGHT,0);
  gwidth=GameParametres(GET,GWIDTH,0);
  ulx=GameParametres(GET,GULX,0);

  Shift(GET,ulx,obj==NULL?0:obj->id,&zoom,&cvx,&cvy);

  ifactor=ulx/(gwidth*zoom);

  if(obj->habitat==H_PLANET){
    objx=obj->in->planet->x;
    objy=obj->in->planet->y;
  }
  else{
    objx=obj->x;
    objy=obj->y;
  }
  
  a=(*x-gwidth/2)*ifactor-cvx+objx;
  b=(gheight-*y-gheight/2)*ifactor-cvy+objy;
  *x=a;
  *y=b;

}


void Real2Sector(int x,int y,int *a,int *b){
  /*
    convert the real coordinates x,y in sectors a,b.
  */

  *a=x/SECTORSIZE-(x<0);
  *b=y/SECTORSIZE-(y<0);
}


void DrawSelectionBox(GdkPixmap *pixmap,GdkGC *color,Space reg,Object *cv){
  Rectangle rect;

  int x0,y0,x1,y1;

  if(reg.habitat<0){
    fprintf(stderr,"ERROR: DrawSelectionBox(): reb habitat=%d (<0)\n",reg.habitat);
    return;
  }
  rect.x=reg.rect.x;
  rect.y=reg.rect.y;
  rect.width=reg.rect.width;
  rect.height=reg.rect.height;

  if(reg.habitat==0 && keys.mleft==FALSE){


    Real2Window(cv,reg.habitat,rect.x,rect.y,&x0,&y0);
    Real2Window(cv,reg.habitat,rect.x+rect.width,rect.y+rect.height,&x1,&y1);
    
    rect.x=x0;
    rect.y=y0;
    rect.width=x1-x0;
    rect.height=y1-y0;
  }

  if(reg.habitat>0 && keys.mleft==FALSE){
    Real2Window(cv,reg.habitat,rect.x,rect.y,&x0,&y0);
    Real2Window(cv,reg.habitat,rect.x+rect.width,rect.y+rect.height,&x1,&y1);
    
    rect.x=x0;
    rect.y=y0;
    rect.width=x1-x0;
    rect.height=y1-y0;
    
    rect.y=GameParametres(GET,GHEIGHT,0)-rect.y;
  }

  if(rect.width<0){
    rect.x+=rect.width;
    rect.width*=-1;
  }
  
  if(rect.height<0){
    rect.y+=rect.height;
    rect.height*=-1;
  }
  
  gdk_draw_rectangle(pixmap,penGreen,FALSE,rect.x,rect.y,rect.width,rect.height);
}



void DrawGameStatistics(GdkPixmap *pixmap,struct Player *pl){
  /*
    Show general game statistics
   */

  int nplayers;
  int i=0;
  int x,y;
  int gwidth,gheight;
  char cad[MAXTEXTLEN];
  static int len0=0;
  static int textwidth=0;
  static int textheight=0;
  int len;

  if(gfont==NULL)return;
  nplayers=GameParametres(GET,GNPLAYERS,0)+2;
  gwidth=GameParametres(GET,GWIDTH,0);
  gheight=GameParametres(GET,GHEIGHT,0);

  //  printf("LEN:%d %d %d\n",len0,textwidth,textheight);

  x=gwidth/2-textwidth/2;
  y=3*gheight/4-nplayers*8;
  y=2;

  gdk_draw_rectangle(pixmap,    
		     penBlack,
		     TRUE,   
		     x-10,
		     y,
		     textwidth+20,
		     textheight*(nplayers)+4);

  gdk_draw_rectangle(pixmap,    
		     penGreen,
		     FALSE,   
		     x-10,
		     y,
		     textwidth+20,
		     textheight*(nplayers)+4);


  snprintf(cad,MAXTEXTLEN,"Game statistics:");
  DrawString(pixmap,gfont,penWhite,x,y+textheight,cad);

  /* HERE: send kills and deaths to client */

  for(i=1;i<nplayers;i++){
    snprintf(cad,MAXTEXTLEN,"player: %s, [L%d : %d : %1.1f], ships: %d (%d), planets: %d, kills: %d, deaths: %d.",
	     pl[i].playername,pl[i].maxlevel,pl[i].level,(float)pl[i].level/(pl[i].nships+0.0001),
	     pl[i].nships,pl[i].nbuildships,
	     pl[i].nplanets,pl[i].nkills,
	     pl[i].ndeaths);
    len=strlen(cad);
    if(len>len0){
      len0=len;
      textwidth=gdk_text_width(gfont,cad,len0);
      textheight=gdk_text_height(gfont,cad,len0);
    }
    DrawString(pixmap,gfont,gcolors[players[i].color],x,y+(i+1)*textheight,cad);
  }
}


void DrawString(GdkDrawable *pixmap,GdkFont *font,GdkGC *gc,gint x,gint y,const gchar *string){
  /*
    
    
   */
  if(font==NULL)return;
  if(pixmap==NULL)return;
  if(gc==NULL)return;
  if(string==NULL)return;

  gdk_draw_string(pixmap,font,gc,x,y,string);
}


void DrawMessageBox(GdkPixmap *pixmap,GdkFont *font,char *cad,int x0,int y0,int type){
  /*
    draw a text message centered at x0,y0
*/
  int x,y,width,height;
  int textw,texth;

  if(font!=NULL){
    textw=gdk_text_width(font,cad,strlen(cad));
    texth=gdk_text_height(font,cad,strlen(cad));
  }
  else{
    texth=12;
    textw=12;
  }
  width=textw+2*texth;
  height=2*texth;

  x=x0-width/2;
  y=y0-height/2;
  if(x<10)x=10;
  
  gdk_draw_rectangle(pixmap,    
		     penBlack,
		     TRUE,   
		     x,
		     y,
		     width,
		     height);
  switch(type){
  case MBOXDEFAULT:
    break;
  case MBOXBORDER:
    gdk_draw_rectangle(pixmap,    
		       penGreen,
		       FALSE,   
		       x,
		       y,
		       width,
		       height);
    break;
  default:
    break;

  }  

  DrawString(pixmap,font,penGreen,x+texth,y+1.5*texth,cad);
}



void DrawCharList (GdkPixmap *pixmap,GdkFont *font,GdkGC *color,struct CharListHead *hlist,int x0,int y0){

  /* 
     version 00
     print the list to screen
     returns:
     the number of item printed.
   */

  struct CharList *list;
  int n=0; 
  int i;
  int incy;
  int textw;
  char point[MAXTEXTLEN];
  static int charh=10;
  static int swgmess=0;
  static int glen=0;
  static int sw=0;


  if(sw==0){
    if(font!=NULL){
      charh=gdk_text_height(font,"pL",2);
    }
    else{
      charh=12;
    }
    sw++;
  }

  incy=charh;

  if(swgmess){
    gdk_draw_rectangle(pixmap,    
		       penBlack,
		       TRUE,   
		       x0,
		       y0,
		       glen+25,
		       incy*(swgmess+1));
    
    gdk_draw_rectangle(pixmap,    
		       penRed,
		       FALSE,   
		       x0,
		       y0,
		       glen+25,
		       incy*(swgmess+1));

    gdk_draw_line(pixmap,    
		  penRed,
		  x0+glen+10,
		  y0,
		  x0+glen+10,
		  y0+incy*(swgmess+1));
  }

  if(hlist==NULL){
    fprintf(stderr,"ERROR in DrawCharList(): NULL\n");
    exit(-1);
  }
  i=0;
  list=hlist->next;
  glen=0;
  while(list!=NULL){
    if(i<hlist->first){list=list->next;i++;continue;}
    if(i<hlist->n-20){list=list->next;i++;continue;}

    strncpy(point,list->cad,MAXTEXTLEN);
    DrawString(pixmap,font,color,x0+5,y0+incy*swgmess+5,point);
    textw=gdk_text_width(font,point,strlen(point));    
    if(textw>glen)glen=textw;
    

    //    printf("%d:  %s\n",n,list->cad);
    list=list->next;
    i++;n++;
    swgmess=n;
  }
  if(i!=hlist->n){
    printf("ERROR in DrawCharList n: %d  hlist.n:%d %d %d\n",i,hlist->n,n,hlist->first);
  }
  return;
}

void DrawBarBox (GdkPixmap *pixmap,GdkGC *color,int x,int y,int w,int h,float value){

  int n;
  n=value*w;
  gdk_draw_rectangle(pixmap,color,TRUE,x,y,n,h);
  gdk_draw_rectangle(pixmap,color,FALSE,x,y,w,h);

}

void DrawWindow(GdkPixmap *pixmap,GdkFont *font,GdkGC *color0,int x0,int y0,int type,struct Window *w){
  struct CharListHead *hlist;
  struct CharList *list;
  GdkGC *color;
  int n=0; 
  int i;
  int wheight;
  int incy;
  int textw;
  char point[MAXTEXTLEN];
  struct Window window;
  static int charh=10;
  static int swgmess=0;
  static int glen=0;
  static int sw=0;
  int first,last,max,scrolln,dif;


  if(sw==0){
    if(font!=NULL){
      charh=gdk_text_height(font,"pL",2);
    }
    else{
      charh=12;
    }
    sw++;
  }
  
  window.type=0;
  window.x=x0;
  window.y=y0;
  window.width=0;
  window.height=0;

  incy=charh;
  wheight=GameParametres(GET,GHEIGHT,0);

  
  switch(type){
  case 0:
    break;
  case 1:  /* window with scroll */
    hlist=(struct CharListHead *)w->data;


    scrolln=hlist->n - w->scrollbar.n;  /* scrolln item: lighted */
    if(scrolln <0){
      scrolln=0;
      w->scrollbar.n=hlist->n;
    }

    if(swgmess){

      window.x=x0;
      window.y=y0;
      window.width=glen+25;
      window.height=incy*(swgmess+1);


      
      gdk_draw_rectangle(pixmap,
			 penBlack,
			 TRUE,   
			 window.x,
			 window.y, 
			 window.width,
			 window.height);
      
      gdk_draw_rectangle(pixmap,    
			 penRed,
			 FALSE,   
			 window.x, 
			 window.y, 
			 window.width,
			 window.height);
      
      /* gdk_draw_line(pixmap, */
      /* 		    penRed, */
      /* 		    x0+window.width-w->scrollbar.width, */
      /* 		    y0, */
      /* 		    x0+window.width-w->scrollbar.width, */
      /* 		    y0+window.height); */
      {
	int xbar,ybar,wbar,hbar;
	int i;



	xbar=x0+window.width-w->scrollbar.width;
	wbar=w->scrollbar.width;
	hbar=window.height-10;
	ybar=1.0*hbar*(scrolln)/hlist->n+5;

	/* cursor */
	for(i=0;i<=5;i++){
	  gdk_draw_line(pixmap,penRed,
			xbar,ybar+y0-5+i*2,
			xbar+wbar,ybar+y0-5+i*2);
	}
	/* arrows */
	gdk_draw_line(pixmap,penRed, 
		      xbar+2,y0+8,
		      xbar+wbar/2,y0+2);
	gdk_draw_line(pixmap,penRed, 
		      xbar+wbar/2,y0+2,
		      xbar+wbar-2,y0+8);

	gdk_draw_line(pixmap,penRed, 
		      xbar+2,y0+window.height-8,
		      xbar+wbar/2,y0+window.height);
	gdk_draw_line(pixmap,penRed, 
		      xbar+wbar/2,y0+window.height,
		      xbar+wbar-2,y0+window.height-8);

      }
      w->x=window.x;
      w->y=window.y;
      w->width=window.width;
      w->height=window.height;
    }
    glen=0;
    
    if(hlist==NULL){
      fprintf(stderr,"ERROR in DrawWindow(): NULL\n");
      exit(-1);
    }
    i=0;
    list=hlist->next;

    glen=0;
    first=0;             /* first item to draw */
    last=0;              /* last item 2 draw */
    max=wheight/incy-14; /* max n items*/
    if(max<5)max=5;

    dif=0;
    last =scrolln+max/2;
    if(last>hlist->n){
      last=hlist->n;
      dif=scrolln+max/2-last;
    }

    first=scrolln-max/2-dif;
    if(first<0){
      first=0;
      dif=max/2+dif-scrolln;
    }
    else{
      dif=0;
    }

    if(dif){
      last+=dif;
      if(last>hlist->n)last=hlist->n;
    }
    if(scrolln>last)last=scrolln;

    swgmess=0;

    while(list!=NULL){
      if(i<first){list=list->next;i++;continue;}
      if(i>last){return;}

      strncpy(point,list->cad,MAXTEXTLEN);
      color=color0;
      //      printf("%d %d %d %d\n",i,last,scrolln, w->scrollbar.n);
      if(i==scrolln){
	color=penRed;
      }
      DrawString(pixmap,font,color,x0+5,y0+incy*(swgmess+1)+5,point);
      textw=gdk_text_width(font,point,strlen(point));    
      if(textw>glen)glen=textw;
      
      i++;n++;
      swgmess=n;
      list=list->next;
    }
    if(i!=hlist->n){
      printf("ERROR in DrawWindow n: %d  hlist.n:%d %d %d\n",i,hlist->n,n,hlist->first);
    }
    
    break;
  default:
    return;
  }
  return;
}


int WindowFocus(struct Window *w){
  int x,y;
  if(w->active==FALSE)return(FALSE);
  MousePos(GET,&x,&y);
  if(x > w->x && x < w->x+w->width){
    if(y > w->y && y < w->y+w->height){
      return(TRUE);
    }
  }
  return(FALSE);
}



int ActWindow(struct Window *w){
  int x,y;
  static int n=0;
  int swarrow=0;
  if(w->active==FALSE)return(FALSE);
  MousePos(GET,&x,&y);

  n++; 
  
  if(w->scrollbar.active){
    //    if(x > w->x+w->width-w->scrollbar.width && x < w->x+w->width){
    if(x > w->x && x < w->x+w->width){
      if(y > w->y && y < w->y+w->height){
	
	if(y > w->y && y < w->y+w->scrollbar.width ){
	  swarrow++;
	  w->scrollbar.n++;
	  /* if(w->scrollbar.n>99)w->scrollbar.n=99; */
	}
	if(y <  w->y+w->height && y >  w->y+w->height - w->scrollbar.width ){
	  swarrow++;
	  w->scrollbar.n--;
	  if(w->scrollbar.n<0)w->scrollbar.n=0;
	}
	if(!swarrow){
	  if(y > w->y && y < w->y+w->height/2 ){
	    w->scrollbar.n+=10;
	    /* if(w->scrollbar.n>99)w->scrollbar.n=99; */
	  }
	  if(y <  w->y+w->height && y >  w->y+w->height - w->height/2 ){
	    w->scrollbar.n-=10;
	    if(w->scrollbar.n<0)w->scrollbar.n=0;
	  }
	}
      }
    }
  }
  return(FALSE);
}

void PrintFontNames(int n){
  char **fnames;
  int nfonts;
  int i;

  fnames=XListFonts(GDK_DISPLAY(),"*",n,&nfonts);

  for(i=0;i<nfonts;i++){
    printf("%d     %s \n",i,fnames[i]);
  }

  XFreeFontNames(fnames);

}

void PrintFontName(char *fname,int n){
  char **fnames;
  int nfonts;
  int i;

  fnames=XListFonts(GDK_DISPLAY(),fname,n,&nfonts);

  for(i=0;i<nfonts;i++){
    printf("%s",fnames[i]);
  }

  XFreeFontNames(fnames);

}
