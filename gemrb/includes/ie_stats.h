/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header: /data/gemrb/cvs2svn/gemrb/gemrb/gemrb/includes/ie_stats.h,v 1.41 2006/01/05 21:46:31 avenger_teambg Exp $
 *
 */

/**
 * @file ie_stats.h
 * Definitions of creature stats codes
 * @author The GemRB Project
 */


// !!! NOTE: keep this file synchronized with gemrb/GUIScripts/ie_stats.py !!!

#ifndef IE_STATS_H
#define IE_STATS_H

//EA values
#define EA_INANIMATE   		1
#define EA_PC  			2
#define EA_FAMILIAR		3
#define EA_ALLY			4
#define EA_CONTROLLED  		5
#define EA_CHARMED 		6
#define EA_GOODBUTRED  		28
#define EA_GOODBUTBLUE 		29
#define EA_GOODCUTOFF  		30
#define EA_NOTGOOD 		31
#define EA_ANYTHING		126
#define EA_NEUTRAL 		128
#define EA_NOTEVIL 		199
#define EA_EVILCUTOFF  		200
#define EA_EVILBUTGREEN		201
#define EA_EVILBUTBLUE 		202
#define EA_ENEMY   		255

//GENERAL values
#define GEN_HUMANOID  1  //charm?
#define GEN_ANIMAL    2  //charm animals
#define GEN_DEAD      3  //???
#define GEN_UNDEAD    4  //turn
#define GEN_GIANT     5  //???
#define GEN_FROZEN    6  //???
#define GEN_MONSTER   255

//alignment values
#define AL_GNE_MASK  3  //good neutral evil
#define AL_LNC_MASK  0x30 //lawful neutral evil

//state bits (IE_STATE)
#define STATE_SLEEP      0x00000001
#define STATE_BERSERK    0x00000002
#define STATE_PANIC      0x00000004
#define STATE_STUNNED    0x00000008
#define STATE_INVISIBLE  0x00000010
#define STATE_HELPLESS   0x00000020
#define STATE_FROZEN     0x00000040
#define STATE_PETRIFIED  0x00000080
#define STATE_D3         0x00000100
#define STATE_D4         0x00000200
#define STATE_D5         0x00000400
#define STATE_DEAD       0x00000800
#define STATE_SILENCED   0x00001000
#define STATE_CHARMED    0x00002000
#define STATE_POISONED   0x00004000
#define STATE_HASTED     0x00008000
#define STATE_SLOWED     0x00010000
#define STATE_INFRA      0x00020000
#define STATE_BLIND      0x00040000
//this appears to be a mistake in the original state.ids
//this flag is the 'deactivate' flag, Activate/Deactivate works on it
#define STATE_DISEASED   0x00080000
#define STATE_DEACTIVATED 0x00080000
#define STATE_FEEBLE     0x00100000
#define STATE_NONDET     0x00200000
#define STATE_INVIS2     0x00400000
#define STATE_BLESS      0x00800000
#define STATE_CHANT      0x01000000
#define STATE_HOLY       0x02000000
#define STATE_LUCK       0x04000000
#define STATE_AID        0x08000000
#define STATE_CHANTBAD   0x10000000
#define STATE_BLUR       0x20000000
#define STATE_MIRROR     0x40000000
#define STATE_CONFUSED   0x80000000

#define STATE_CANTMOVE   0x80080fef
#define STATE_CANTLISTEN 0x80080fef
#define STATE_CANTSTEAL  0x00080fc0 //can't steal from
#define STATE_CANTSEE    0x00080fc0 //can't explore (even itself)
#define STATE_NOSAVE     0x00000fc0 //don't save these

//Multiclass flags
#define MC_REMOVE_CORPSE        0x0002
#define MC_KEEP_CORPSE          0x0004
#define MC_WAS_FIGHTER		0x0008
#define MC_WAS_MAGE		0x0010
#define MC_WAS_CLERIC		0x0020
#define MC_WAS_THIEF		0x0040
#define MC_WAS_DRUID		0x0080
#define MC_WAS_RANGER		0x0100
#define MC_FALLEN_PALADIN	0x0200
#define MC_FALLEN_RANGER	0x0400

#define MC_BEENINPARTY          0x8000

#define MC_NO_TALK              0x80000    //ignore dialoginterrupt

//stats
#define IE_HITPOINTS		0
#define IE_MAXHITPOINTS		1
#define IE_ARMORCLASS		2
#define IE_ACCRUSHINGMOD	3
#define IE_ACMISSILEMOD		4
#define IE_ACPIERCINGMOD	5
#define IE_ACSLASHINGMOD	6
#define IE_THAC0		7
#define IE_NUMBEROFATTACKS	8
#define IE_SAVEVSDEATH		9
#define IE_SAVEVSWANDS		10
#define IE_SAVEVSPOLY		11
#define IE_SAVEVSBREATH		12
#define IE_SAVEVSSPELL		13
#define IE_SAVEFORTITUDE 9
#define IE_SAVEREFLEX 10
#define IE_SAVEWILL 11
#define IE_RESISTFIRE		14
#define IE_RESISTCOLD		15
#define IE_RESISTELECTRICITY	16
#define IE_RESISTACID		17
#define IE_RESISTMAGIC		18
#define IE_RESISTMAGICFIRE	19
#define IE_RESISTMAGICCOLD	20
#define IE_RESISTSLASHING	21
#define IE_RESISTCRUSHING	22
#define IE_RESISTPIERCING	23
#define IE_RESISTMISSILE	24
#define IE_LORE		25
#define IE_LOCKPICKING	26
#define IE_STEALTH		27
#define IE_TRAPS		28
#define IE_PICKPOCKET	29
#define IE_FATIGUE		30
#define IE_INTOXICATION	31
#define IE_LUCK		32
#define IE_TRACKING	33
#define IE_LEVEL		34
#define IE_LEVELFIGHTER  34 //for pst, iwd2
#define IE_SEX		35
#define IE_STR		36
#define IE_STREXTRA	37
#define IE_INT		38
#define IE_WIS		39
#define IE_DEX		40
#define IE_CON		41
#define IE_CHR		42
#define IE_XPVALUE		43
#define IE_XP		44
#define IE_GOLD		45
#define IE_MORALEBREAK	46
#define IE_MORALERECOVERYTIME	47
#define IE_REPUTATION	48
#define IE_HATEDRACE	49
#define IE_DAMAGEBONUS	50
#define IE_SPELLFAILUREMAGE	51 
#define IE_SPELLFAILUREPRIEST	52 
#define IE_SPELLDURATIONMODMAGE	53
#define IE_SPELLDURATIONMODPRIEST	54
#define IE_TURNUNDEADLEVEL	55
#define IE_BACKSTABDAMAGEMULTIPLIER	56
#define IE_LAYONHANDSAMOUNT	57
#define IE_HELD 		   58
#define IE_POLYMORPHED     59
#define IE_TRANSLUCENT     60
#define IE_IDENTIFYMODE    61
#define IE_ENTANGLE	62
#define IE_SANCTUARY	63
#define IE_MINORGLOBE	64
#define IE_SHIELDGLOBE	65
#define IE_GREASE		66
#define IE_WEB		67
#define IE_LEVEL2   	   68
#define IE_LEVELMAGE     68 //pst, iwd2
#define IE_LEVEL3   	   69
#define IE_LEVELTHIEF    69 //pst, iwd2
#define IE_CASTERHOLD	70
#define IE_ENCUMBRANCE    71
#define IE_MISSILETHAC0BONUS	  72  
#define IE_MAGICDAMAGERESISTANCE  73
#define IE_RESISTPOISON 	 74
#define IE_DONOTJUMP		  75
#define IE_AURACLEANSING	  76
#define IE_MENTALSPEED  	  77
#define IE_PHYSICALSPEED	  78
#define IE_CASTINGLEVELBONUSMAGE	79
#define IE_CASTINGLEVELBONUSCLERIC  80
#define IE_SEEINVISIBLE 			81
#define IE_IGNOREDIALOGPAUSE		82
#define IE_MINHITPOINTS		 83
#define IE_THAC0BONUSRIGHT  		84
#define IE_THAC0BONUSLEFT   		85
#define IE_DAMAGEBONUSRIGHT 		86
#define IE_DAMAGEBONUSLEFT  		87
#define IE_STONESKINS  			88
#define IE_PROFICIENCYBASTARDSWORD		89
#define IE_PROFICIENCYLONGSWORD		90
#define IE_PROFICIENCYSHORTSWORD		91
#define IE_PROFICIENCYAXE			92
#define IE_PROFICIENCYTWOHANDEDSWORD	93
#define IE_PROFICIENCYKATANA		94
#define IE_PROFICIENCYSCIMITARWAKISASHININJATO	95
#define IE_PROFICIENCYDAGGER		96
#define IE_PROFICIENCYWARHAMMER	97
#define IE_PROFICIENCYSPEAR	98
#define IE_PROFICIENCYHALBERD		99
#define IE_PROFICIENCYFLAILMORNINGSTAR	100
#define IE_PROFICIENCYMACE			101
#define IE_PROFICIENCYQUARTERSTAFF		102
#define IE_PROFICIENCYCROSSBOW			103
#define IE_PROFICIENCYLONGBOW			104
#define IE_PROFICIENCYSHORTBOW			105
#define IE_PROFICIENCYDART			106
#define IE_PROFICIENCYSLING			107
#define IE_PROFICIENCYBLACKJACK			108
#define IE_PROFICIENCYGUN			109
#define IE_PROFICIENCYMARTIALARTS		110
#define IE_PROFICIENCY2HANDED		   	111 
#define IE_PROFICIENCYSWORDANDSHIELD		112
#define IE_PROFICIENCYSINGLEWEAPON		113
#define IE_PROFICIENCY2WEAPON		 114  
#define IE_EXTRAPROFICIENCY1 		 115
#define IE_ALCHEMY               115
#define IE_EXTRAPROFICIENCY2 		 116
#define IE_ANIMALS               116
#define IE_EXTRAPROFICIENCY3 		 117
#define IE_BLUFF                 117
#define IE_EXTRAPROFICIENCY4 		 118
#define IE_CONCENTRATION         118
#define IE_EXTRAPROFICIENCY5 		 119
#define IE_DIPLOMACY             119
#define IE_EXTRAPROFICIENCY6 		 120
#define IE_INTIMIDATE            120
#define IE_EXTRAPROFICIENCY7 		 121
#define IE_SEARCH                121
#define IE_EXTRAPROFICIENCY8 		 122
#define IE_SPELLCRAFT            122
#define IE_EXTRAPROFICIENCY9 		 123
#define IE_MAGICDEVICE           123
#define IE_EXTRAPROFICIENCY10 		 124
#define IE_EXTRAPROFICIENCY11 		 125
#define IE_EXTRAPROFICIENCY12 		 126
#define IE_EXTRAPROFICIENCY13 		 127
#define IE_EXTRAPROFICIENCY14 		 128
#define IE_EXTRAPROFICIENCY15 		 129
#define IE_EXTRAPROFICIENCY16 		 130
#define IE_EXTRAPROFICIENCY17 		 131
#define IE_FEATS1                  131
#define IE_EXTRAPROFICIENCY18 		 132
#define IE_FEATS2                  132
#define IE_EXTRAPROFICIENCY19 		 133
#define IE_FEATS3                  133
#define IE_EXTRAPROFICIENCY20 		 134
#define IE_FREESLOTS	 		 134 //same as above
#define IE_HIDEINSHADOWS			  135
#define IE_DETECTILLUSIONS  		  136
#define IE_SETTRAPS 				  137
#define IE_PUPPETMASTERID   		  138
#define IE_PUPPETMASTERTYPE 		  139
#define IE_PUPPETTYPE   			  140
#define IE_PUPPETID 				  141
#define IE_CHECKFORBERSERK  		  142
#define IE_BERSERKSTAGE1			  143
#define IE_BERSERKSTAGE2			  144
#define IE_DAMAGELUCK   			  145
#define IE_CRITICALHITBONUS 		  146
#define IE_VISUALRANGE  			  147
#define IE_EXPLORE  				  148
#define IE_THRULLCHARM  			  149
#define IE_SUMMONDISABLE			  150
#define IE_HITBONUS 				  151    
#define IE_KIT  				  152 
#define IE_FORCESURGE   			  153
#define IE_SURGEMOD 				  154
#define IE_IMPROVEDHASTE			  155
#define IE_INTERNAL_0            156
#define IE_INTERNAL_1            157
#define IE_INTERNAL_2            158
#define IE_INTERNAL_3            159
#define IE_INTERNAL_4            160
#define IE_INTERNAL_5            161
#define IE_INTERNAL_6            162
#define IE_INTERNAL_7            163
#define IE_INTERNAL_8            164
#define IE_INTERNAL_9            165
#define IE_SCRIPTINGSTATE1  		  156
#define IE_SCRIPTINGSTATE2  		  157
#define IE_SCRIPTINGSTATE3  		  158
#define IE_SCRIPTINGSTATE4  		  159
#define IE_SCRIPTINGSTATE5  		  160
#define IE_SCRIPTINGSTATE6  		  161
#define IE_SCRIPTINGSTATE7  	162   
#define IE_SCRIPTINGSTATE8  	163
#define IE_SCRIPTINGSTATE9  	164
#define IE_SCRIPTINGSTATE10 	165
#define IE_MELEETHAC0		166
#define IE_MELEEDAMAGE		167
#define IE_MISSILEDAMAGE	168
#define IE_NOCIRCLE		169
#define IE_FISTTHAC0		170
#define IE_FISTDAMAGE		171
#define IE_TITLE1		172
#define IE_TITLE2		173
#define IE_DISABLEOVERLAY	174
#define IE_DISABLEBACKSTAB	175
#define IE_XP_MAGE              176 // XP2 
#define IE_XP_THIEF             177 // XP3
#define IE_DIALOGRANGE          178 // iwd2
#define IE_MOVEMENTRATE         179
#define IE_MORALE               180 // this has no place
//
#define IE_ENABLEOFFSCREENAI    184 // bg2 has this on this spot
//temporarily here for iwd2
#define IE_HATEDRACE2       192
#define IE_HATEDRACE3       193
#define IE_HATEDRACE4       194
#define IE_HATEDRACE5       195
#define IE_HATEDRACE6       196
#define IE_HATEDRACE7       197
#define IE_HATEDRACE8       198
//
#define IE_STONESKINSGOLEM	199
#define IE_LEVELDRAIN		200
//GemRB Specific Defines
#define IE_RACE			201
#define IE_CLASS		202
#define IE_GENERAL		203
#define IE_EA			204
#define IE_SPECIFIC		205

#define IE_ANIMATION_ID		206
#define IE_STATE_ID		207
#define IE_METAL_COLOR		208
#define IE_COLORS		208 //same
#define IE_MINOR_COLOR		209
#define IE_MAJOR_COLOR		210
#define IE_SKIN_COLOR		211
#define IE_LEATHER_COLOR	212
#define IE_ARMOR_COLOR		213
#define IE_HAIR_COLOR		214
#define IE_COLORCOUNT		214 //same
#define IE_MC_FLAGS		215
#define IE_CLASSLEVELSUM	216 //iwd2
#define IE_ALIGNMENT		217
#define IE_UNSELECTABLE		218
#define IE_ARMOR_TYPE		219
#define IE_TEAM			220
#define IE_FACTION		221
#define IE_SUBRACE		222
#define IE_SPECIES              223
//probably these should be stored in the spellbook structure
#define IE_PRIESTBONUS1         224
#define IE_PRIESTBONUS2         225
#define IE_PRIESTBONUS3         226
#define IE_PRIESTBONUS4         227
#define IE_PRIESTBONUS5         228
#define IE_PRIESTBONUS6         229
#define IE_PRIESTBONUS7         230
#define IE_WIZARDBONUS1         231
#define IE_WIZARDBONUS2         232
#define IE_WIZARDBONUS3         233
#define IE_WIZARDBONUS4         234
#define IE_WIZARDBONUS5         235
#define IE_WIZARDBONUS6         236
#define IE_WIZARDBONUS7         237
#define IE_WIZARDBONUS8         238
#define IE_WIZARDBONUS9         239
//These are in IWD2, but in a different place
#define IE_LEVELBARBARIAN        240 //iwd2 class levels (fighter, mage, thief are already stored)
#define IE_LEVELBARD             241 
#define IE_LEVELCLERIC           242 
#define IE_LEVELDRUID            243
#define IE_LEVELMONK             244
#define IE_LEVELPALADIN          245
#define IE_LEVELRANGER           246
#define IE_LEVELSORCEROR         247

#define IE_SAVEDXPOS             253
#define IE_SAVEDYPOS             254
#define IE_SAVEDFACE             255
// place for more classes

#endif  // ! IE_STATS_H
