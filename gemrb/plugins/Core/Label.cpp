/***************************************************************************
                          Label.cpp  -  description
                             -------------------
    begin                : dom ott 12 2003
    copyright            : (C) 2003 by GemRB Developement Team
    email                : Balrog994@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "../../includes/win32def.h"
#include "Label.h"
#include "Interface.h"

#ifndef WIN32
#include <ctype.h>
char *strlwr(char *string)
{
	char *s;
	if(string)
	{
		for(s = string; *s; ++s)
			*s = tolower(*s);
	}
	return string;
}
#endif

extern Interface * core;

Label::Label(unsigned short bLength, Font * font){
	this->font = font;
	Buffer = NULL;
	if(bLength != 0)
		Buffer = (char*)malloc(bLength);
	useRGB = false;
	Alignment = IE_FONT_ALIGN_LEFT;
	palette = NULL;
}
Label::~Label(){
	if(palette)
		free(palette);
	if(Buffer)
		free(Buffer);
}
/** Draws the Control on the Output Display */
void Label::Draw(unsigned short x, unsigned short y)
{
	if(font && Changed) {
		if(useRGB)
			font->Print(Region(this->XPos+x, this->YPos+y, this->Width, this->Height), (unsigned char*)Buffer, palette, Alignment | IE_FONT_ALIGN_MIDDLE | IE_FONT_SINGLE_LINE, true);
		else
			font->Print(Region(this->XPos+x, this->YPos+y, this->Width, this->Height), (unsigned char*)Buffer, NULL, Alignment | IE_FONT_ALIGN_MIDDLE | IE_FONT_SINGLE_LINE, true);
		Changed = false;
	}
}
/** This function sets the actual Label Text */
int Label::SetText(const char * string, int pos)
{
	if(Buffer != NULL) {
		strcpy(Buffer, string);
		if(Alignment == IE_FONT_ALIGN_CENTER)
			strlwr(Buffer);
	}
	Changed = true;
	return 0;
}
/** Sets the Foreground Font Color */
void Label::SetColor(Color col, Color bac)
{
	if(palette)
		free(palette);
	palette = core->GetVideoDriver()->CreatePalette(col, bac);
	Changed = true;
}

void Label::SetAlignment(unsigned char Alignment)
{
	if(Alignment > IE_FONT_ALIGN_RIGHT)
		return;
	this->Alignment = Alignment;
	if(Alignment == IE_FONT_ALIGN_CENTER)
		strlwr(Buffer);
	Changed = true;
}
