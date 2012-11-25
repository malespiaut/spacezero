/*
Copyright (C) 2006 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef _RANDOMNAMEGEN_
#define _RANDOMNAMEGEN_

#ifndef TRUE
#define FALSE 0
#define TRUE 1
#endif


char *createName(int length);
void loadFrequencyTable(char *filename);
char *getRandomName();
char *getRandomName(int length);
char *getRandomFullName();
char *getRandomOrkishName();


/*
Since there can be multiple version of swearwords (such as shithead and fuckface, etc)
the rountine will filter words if the bad word appears at the start of the name generated.
We've also added in "Bob" since we don't want anyone else to be called that!
*/
/* const char badWord[][25] = { */

/* 	"fuck", */
/* 	"shit", */
/* 	"cunt", */
/* 	"rape", */
/* 	"wank", */
/* 	"arse", */
/* 	"nigger", */
/* 	"lesbian", */
/* 	"queer", */
/* 	"gay", */
/* 	"homosexual", */
/* 	"bob" */
/* }; */

#endif
