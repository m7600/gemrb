/***************************************************************************
                          Button.cpp  -  description
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
#include "Button.h"
#include "Interface.h"

#ifndef WIN32
#include <ctype.h>
char *strupr(char *string)
{
	char *s;
	if(string)
	{
		for(s = string; *s; ++s)
			*s = toupper(*s);
	}
	return string;
}
#endif

extern Interface * core;

Button::Button(bool Clear){
	Unpressed = Pressed = Selected = Disabled = NULL;
	this->Clear = Clear;
	State = IE_GUI_BUTTON_UNPRESSED;
	ButtonOnPress[0] = 0;
	Text = (char*)malloc(64);
	hasText = false;
	font = core->GetFont("STONEBIG");
	palette = (Color*)malloc(256*sizeof(Color));
	memcpy(palette, font->GetPalette(), 256*sizeof(Color));
	for(int i = 0; i < 256; i++) {
		palette[i].r = (palette[i].r * 2) / 3;
		palette[i].g = (palette[i].g * 2) / 3;
		palette[i].b = (palette[i].b * 2) / 3;
	}
	Flags = 0x4;
	ToggleState = false;
	Value = 0;
	Picture = NULL;
}
Button::~Button(){
	Video * video = core->GetVideoDriver();
	if(Clear) {
		if(Unpressed)
			video->FreeSprite(Unpressed);
		if(Pressed && (Pressed != Unpressed))
			video->FreeSprite(Pressed);
		if(Selected && ((Selected != Pressed) && (Selected != Unpressed)))
			video->FreeSprite(Selected);
		if(Disabled && ((Disabled != Pressed) && (Disabled != Unpressed) && (Disabled != Selected)))
			video->FreeSprite(Disabled);
	}
	if(Picture)
		video->FreeSprite(Picture);
	free(palette);
}
/** Sets the 'type' Image of the Button to 'img'.
'type' may assume the following values:
- IE_GUI_BUTTON_UNPRESSED
- IE_GUI_BUTTON_PRESSED
- IE_GUI_BUTTON_SELECTED
- IE_GUI_BUTTON_DISABLED */
void Button::SetImage(unsigned char type, Sprite2D * img)
{
	switch(type) {
		case IE_GUI_BUTTON_UNPRESSED:
		{
			if(Unpressed && Clear)
				core->GetVideoDriver()->FreeSprite(Unpressed);
			Unpressed = img;
		}
		break;
		
		case IE_GUI_BUTTON_PRESSED:
    {
			if(Pressed && Clear)
				core->GetVideoDriver()->FreeSprite(Pressed);
			Pressed = img;
		}
		break;
		
		case IE_GUI_BUTTON_SELECTED:
    {
			if(Selected && Clear)
				core->GetVideoDriver()->FreeSprite(Selected);
			Selected = img;
		}
		break;
		
		case IE_GUI_BUTTON_DISABLED:
		{
			if(Disabled && Clear)
				core->GetVideoDriver()->FreeSprite(Disabled);
			Disabled = img;
		}
		break;
	}
	Changed = true;
}
/** Draws the Control on the Output Display */
void Button::Draw(unsigned short x, unsigned short y)
{
	if(!Changed)
		return;
	if(!(Flags & 0x1)) {
		Color white = {0xff, 0xff, 0xff, 0x00}, black = {0x00, 0x00, 0x00, 0x00};
		int align=0;

		if(Flags&0x100) align|=IE_FONT_ALIGN_LEFT;
		else if(Flags&0x200) align|=IE_FONT_ALIGN_RIGHT;
		else align|=IE_FONT_ALIGN_CENTER;
		if(Flags&0x400) align|=IE_FONT_ALIGN_TOP;
	//	else if(Flags&0x800) align|=IE_FONT_ALIGN_BOTTOM;
		else align|=IE_FONT_ALIGN_MIDDLE;
		switch(State) {
			case IE_GUI_BUTTON_UNPRESSED:
			{
				if(Unpressed)
					core->GetVideoDriver()->BlitSprite(Unpressed, x+XPos, y+YPos, true);
				if(hasText)
					font->Print(Region(x+XPos, y+YPos, Width, Height), (unsigned char*)Text, NULL, align | IE_FONT_SINGLE_LINE, true);
				Changed = false;
			}
			break;
		
			case IE_GUI_BUTTON_PRESSED:
			{
				if(Pressed)
					core->GetVideoDriver()->BlitSprite(Pressed, x+XPos, y+YPos, true);
				if(hasText)
					font->Print(Region(x+XPos+2, y+YPos+2, Width, Height), (unsigned char*)Text, NULL, align | IE_FONT_SINGLE_LINE, true);
				Changed = false;
			}	
			break;
		
			case IE_GUI_BUTTON_SELECTED:
			{
				if(Selected)
					core->GetVideoDriver()->BlitSprite(Selected, x+XPos, y+YPos, true);
				else if(Unpressed)
					core->GetVideoDriver()->BlitSprite(Unpressed, x+XPos, y+YPos, true);
				Changed = false;
			}
			break;
		
			case IE_GUI_BUTTON_DISABLED:
			{
				if(Disabled) {
					core->GetVideoDriver()->BlitSprite(Disabled, x+XPos, y+YPos, true);
					if(hasText)
						font->Print(Region(x+XPos, y+YPos, Width, Height), (unsigned char*)Text, palette, IE_FONT_ALIGN_CENTER | IE_FONT_ALIGN_MIDDLE | IE_FONT_SINGLE_LINE, true);
				}
				else if(Unpressed) {
					core->GetVideoDriver()->BlitSprite(Unpressed, x+XPos, y+YPos, true);
					if(hasText)
						font->Print(Region(x+XPos, y+YPos, Width, Height), (unsigned char*)Text, palette, IE_FONT_ALIGN_CENTER | IE_FONT_ALIGN_MIDDLE | IE_FONT_SINGLE_LINE, true);
				}
				Changed = false;
			}
			break;
		}
	}
	if(Picture && (Flags&0x2)) {
		short xOffs = (Width / 2) - (Picture->Width / 2);
		short yOffs = (Height / 2) - (Picture->Height / 2);
		core->GetVideoDriver()->BlitSprite(Picture, x+XPos+xOffs, y+YPos+yOffs, true);
	}
}
/** Sets the Button State */
void Button::SetState(unsigned char state)
{
	if(state > 3) // If wrong value inserted
		return;
	if(State != state)
		Changed = true;
	State = state;
}

/** Mouse Button Down */
void Button::OnMouseDown(unsigned short x, unsigned short y, unsigned char Button, unsigned short Mod)
{
	if((Button == 1) && (State != IE_GUI_BUTTON_DISABLED)) {
		State = IE_GUI_BUTTON_PRESSED;
		Changed = true;
		if(Flags & 0x04) {
			core->GetSoundMgr()->Play("GAM_09");
		}
	}
}
/** Mouse Button Up */
void Button::OnMouseUp(unsigned short x, unsigned short y, unsigned char Button, unsigned short Mod)
{
	if(State == IE_GUI_BUTTON_DISABLED)
		return;
	if(State == IE_GUI_BUTTON_PRESSED)
	{
		if(ToggleState)
			SetState(IE_GUI_BUTTON_SELECTED);
		else
			SetState(IE_GUI_BUTTON_UNPRESSED);
	}
	if((x <= Width) && (y <= Height)) {
		if(Flags & 0x10) {  //checkbox
			ToggleState = !ToggleState;
			if(ToggleState)
				SetState(IE_GUI_BUTTON_SELECTED);
			else
				SetState(IE_GUI_BUTTON_UNPRESSED);
			if(VarName[0] != 0) {
				unsigned long tmp=0;
				core->GetDictionary()->Lookup( VarName, tmp);
				core->GetDictionary()->SetAt(VarName, tmp^Value);
			}
		}
		else if(Flags & 0x20) {  //radio button
			ToggleState = true;
			SetState(IE_GUI_BUTTON_SELECTED);
			if(VarName[0] != 0) {
				core->GetDictionary()->SetAt(VarName, Value);
				((Window*)Owner)->RedrawControls(VarName, Value);
			}
		}
		if(Flags & 0x04) {
			if(Flags & 0x08)
				core->GetSoundMgr()->Play("GAM_04");
			else
				core->GetSoundMgr()->Play("GAM_03");
		}
		if(ButtonOnPress[0] != 0)
			core->GetGUIScriptEngine()->RunFunction(ButtonOnPress);
	}
}

/** Sets the Text of the current control */
int Button::SetText(const char * string, int pos)
{
	if(string == NULL)
		hasText = false;
	else if(string[0] == 0)
		hasText = false;
	else {
		strncpy(Text, string, 63);
		hasText = true;
		Text = strupr(Text);
	}
	Changed = true;
	return 0;
}

/** Set Event */
void Button::SetEvent(char * funcName)
{
	strcpy(ButtonOnPress, funcName);
	Changed = true;
}

/** Sets the Display Flags */
int Button::SetFlags(int arg_flags, int opcode)
{
	switch(opcode)
	{
	case OP_SET:
		Flags = arg_flags;  //set
		break;
	case OP_OR:
		Flags |= arg_flags; //turn on
		break;
	case OP_NAND:
		Flags &= ~arg_flags;//turn off
		break;
	default:
		return -1;
	}
	Changed = true;
	((Window*)Owner)->Invalidate();
	return 0;
}

/** Redraws a button from a given radio button group */
void Button::RedrawButton(char *VariableName, int Sum)
{
	if(strnicmp(VarName, VariableName, MAX_VARIABLE_LENGTH)) return;
	if(Flags&0x20) ToggleState=(Sum==Value);       //radio button, exact value
	else if(Flags&0x10) ToggleState=!!(Sum&Value); //checkbox, bitvalue
	else ToggleState=false;                        //other buttons, no value
	if(ToggleState)
		State = IE_GUI_BUTTON_SELECTED;
	else
		State = IE_GUI_BUTTON_UNPRESSED;
	Changed = true;
}
/** Sets the Picture */
void Button::SetPicture(Sprite2D * Picture)
{
	if(this->Picture)
		core->GetVideoDriver()->FreeSprite(this->Picture);
	this->Picture = Picture;
	Changed = true;
	((Window*)Owner)->Invalidate();
}
