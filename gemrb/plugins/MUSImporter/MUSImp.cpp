/***************************************************************************
                          MUSImp.cpp  -  description
                             -------------------
    begin                : ven ott 24 2003
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
#include "MUSImp.h"
#include "../Core/Interface.h"

MUSImp::MUSImp()
{
	Initialized = false;
	Playing = false;
	str = new FileStream();
	PLpos = 0;
	lastSound = 0xffffffff;
}
MUSImp::~MUSImp(){
	if(str)
		delete(str);
}
/** Initializes the PlayList Manager */
bool MUSImp::Init()
{
	Initialized = true;
	return true;
}
/** Loads a PlayList for playing */
bool MUSImp::OpenPlaylist(const char * name)
{
	playlist.clear();
	PLpos = 0;
	if(stricmp(name, PLName) == 0)
		return true;
	char path[_MAX_PATH];
	strcpy(path, core->GamePath);
	strcat(path, "music");
	strcat(path, SPathDelimiter);
	strcat(path, name);
	strcat(path, ".mus");
	if(!str->Open(path, true))
		return false;
	str->ReadLine(PLName, 32);
	char counts[5];
	str->ReadLine(counts, 5);
	int count = atoi(counts);
	while(count != 0) {
		char line[64];
		str->ReadLine(line, 64);
		int len = strlen(line);
		int i = 0;
		int p = 0;
		PLString pls;
		while(i < len) {
			if((line[i]!=' ') && (line[i]!='\t'))
				pls.PLFile[p++] = line[i++];
			else {
				while(i < len) {
					if((line[i]==' ') || (line[i]=='\t'))
						i++;
					else
						break;
				}
				break;
			}		
		}
		pls.PLFile[p]=0;
		p=0;
		if(line[i]!='@' && (i < len)) {
			while(i < len) {
				if((line[i]!=' ') && (line[i]!='\t'))
					pls.PLTag[p++] = line[i++];
				else
					break;
			}
			pls.PLTag[p]=0;
			p=0;
			i++;
			if((line[i]==' ') || (line[i]=='\t'))
				strcpy(pls.PLLoop, pls.PLTag);
			else {
				while(i < len) {
					if((line[i]!=' ') && (line[i]!='\t'))
						pls.PLLoop[p++] = line[i++];
					else
						break;
				}
			pls.PLLoop[p]=0;
			}
			while(i < len) {
				if((line[i]==' ') || (line[i]=='\t'))
					i++;
				else
					break;
			}
			p=0;
		}
		else {
			pls.PLTag[0]=0;
			pls.PLLoop[0]=0;
		}
		while(i < len) {
			if((line[i]!=' ') && (line[i]!='\t'))
				i++;
			else
				break;
		}
		i++;
		while(i < len) {
			if((line[i]!=' ') && (line[i]!='\t'))
				pls.PLEnd[p++] = line[i++];
			else
				break;
		}
		pls.PLEnd[p]=0;
		char FName[_MAX_PATH];
      	strcpy(FName, core->GamePath);
		strcat(FName, "music");
		strcat(FName, SPathDelimiter);
		strcat(FName, PLName);
		strcat(FName, SPathDelimiter);
		strcat(FName, PLName);
		strcat(FName, pls.PLFile);
		strcat(FName, ".acm");
		pls.soundID = core->GetSoundMgr()->LoadFile(FName);
		playlist.push_back(pls);
		printf("%s.acm Added in position %d\n", pls.PLFile, pls.soundID);
		count--;
	}
	return true;
}
/** Start the PlayList Music Execution */
void MUSImp::Start()
{
	if(!Playing) {
		PLpos = 0;
		core->GetSoundMgr()->Play(playlist[PLpos].soundID);
		lastSound = playlist[PLpos].soundID;
		Playing = true;
	}
}
/** Ends the Current PlayList Execution */
void MUSImp::End()
{
	if(Playing) {
		if(playlist[PLpos].PLEnd[0] != 0) {
			core->GetSoundMgr()->Stop(lastSound);
			for(int i = 0; i < playlist.size(); i++) {
				if(stricmp(playlist[i].PLFile, playlist[PLpos].PLEnd) == 0) {
					core->GetSoundMgr()->Play(playlist[i].soundID);
					PLpos = i;
					return;
				}
			}
			char FName[_MAX_PATH];
			strcpy(FName, core->GamePath);
			strcat(FName, "music");
			strcat(FName, SPathDelimiter);
			strcat(FName, PLName);
			strcat(FName, SPathDelimiter);
			strcat(FName, PLName);
			strcat(FName, playlist[PLpos].PLEnd);
			strcat(FName, ".acm");
			core->GetSoundMgr()->Stop(lastSound);
			lastSound = core->GetSoundMgr()->LoadFile(FName);
			core->GetSoundMgr()->Play(lastSound);
		}
		else
			core->GetSoundMgr()->Stop(lastSound);
	}
}
/** Switches the current PlayList while playing the current one */
void MUSImp::SwitchPlayList(const char * name){
}
/** Plays the Next Entry */
void MUSImp::PlayNext()
{
	if(playlist[PLpos].PLLoop[0] != 0) {
		printf("Looping...\n");
		for(int i = 0; i < playlist.size(); i++) {
			if(stricmp(playlist[i].PLFile, playlist[PLpos].PLLoop) == 0) {
				PLpos = i;
				break;
			}
		}
		lastSound = playlist[PLpos].soundID;
		core->GetSoundMgr()->Play(lastSound);
	}
	else {
		printf("Next Track...\n");
		PLpos++;
		lastSound = playlist[PLpos].soundID;
		core->GetSoundMgr()->Play(lastSound);
	}
}
