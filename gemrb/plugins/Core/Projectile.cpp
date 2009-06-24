/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2006 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $Id$
 *
 */

#include <cmath>
#include "../../includes/win32def.h"
#include <stdlib.h>
#include "Projectile.h"
#include "Interface.h"
#include "Video.h"
#include "Game.h"
#include "ResourceMgr.h"
#include "Audio.h"
#include "ProjectileServer.h"

extern Interface* core;
#ifdef WIN32
extern HANDLE hConsole;
#endif

//to get gradient color
#define PALSIZE 12

static const ieByte SixteenToNine[MAX_ORIENT]={0,1,2,3,4,5,6,7,8,7,6,5,4,3,2,1};
static const ieByte SixteenToFive[MAX_ORIENT]={0,1,2,3,4,3,2,1,0,1,2,3,4,3,2,1};

Projectile::Projectile()
{
	autofree = false;
	Extension = NULL;
	area = NULL;
	palette = NULL;
	PaletteRes[0]=0;
	Pos.empty();
	Destination = Pos;
	Orientation = 0;
	NewOrientation = 0;
	path = NULL;
	step = NULL;
	timeStartStep = 0;
	phase = P_UNINITED;
	effects = NULL;
	children = NULL;
	child_size = 0;
	memset(travel, 0, sizeof(travel)) ;
	memset(shadow, 0, sizeof(shadow)) ;
	light = NULL ;
}

Projectile::~Projectile()
{
	int i;

	if (autofree) {
		free(Extension);
	}
	delete effects;

	gamedata->FreePalette(palette, PaletteRes);
	ClearPath();

	if (phase != P_UNINITED) {
		for (i = 0; i < MAX_ORIENT; ++i) {
			if(travel[i])
				delete travel[i];
			if(shadow[i])
				delete shadow[i];
		}
		core->GetVideoDriver()->FreeSprite(light);
	}

	if(children) {
		for(i=0;i<child_size;i++) {
			delete children[i];
		}
		free (children);
	}
}

void Projectile::InitExtension()
{
	autofree = false;
	if (!Extension) {
		Extension = (ProjectileExtension *) calloc( 1, sizeof(ProjectileExtension));
	}
}

void Projectile::CreateAnimations(Animation **anims, const ieResRef bamres, int Seq)
{
	AnimationFactory* af = ( AnimationFactory* )
		core->GetResourceMgr()->GetFactoryResource( bamres,
		IE_BAM_CLASS_ID, IE_NORMAL );
	//
	if (!af) {
		return;
	}
	//this hack is needed because bioware .pro files are sometimes
	//reporting bigger face count than possible by the animation
	int Max = af->GetCycleCount();
	if (Aim>Max) Aim=Max;

	if(ExtFlags&PEF_PILLAR) {
		CreateCompositeAnimation(anims, af, Seq);
	} else {
		CreateOrientedAnimations(anims, af, Seq);
	}
}

//Seq is the first cycle to use in the composite
//Aim is the number of cycles
void Projectile::CreateCompositeAnimation(Animation **anims, AnimationFactory *af, int Seq)
{
	for (int Cycle = 0; Cycle<Aim; Cycle++) {
		int c = Cycle+Seq;
		Animation* a = af->GetCycle( c );
		anims[Cycle] = a;
		if (!a) continue;
		//animations are started at a random frame position
		//Always start from 0, unless set otherwise
		if (!(ExtFlags&PEF_RANDOM)) {
			a->SetPos(0);
		}

		a->gameAnimation = true;
	}
}

//Seq is the cycle to use in case of single orientations
//Aim is the number of Orientations
void Projectile::CreateOrientedAnimations(Animation **anims, AnimationFactory *af, int Seq)
{
	for (int Cycle = 0; Cycle<MAX_ORIENT; Cycle++) {
		bool mirror = false, mirrorvert = false;
		int c;
		switch(Aim) {
		default:
			c = Seq;
			break;
		case 5:
			c = SixteenToFive[Cycle];
			// orientations go counter-clockwise, starting south
			if (Cycle <= 4) {
				// bottom-right quadrant
				mirror = false; mirrorvert = false;
			} else if (Cycle <= 8) {
				// top-right quadrant
				mirror = false; mirrorvert = true;
			} else if (Cycle < 12) {
				// top-left quadrant
				mirror = true; mirrorvert = true;
			} else {
				// bottom-left quadrant
				mirror = true; mirrorvert = false;
			}
			break;
		case 9:
			c = SixteenToNine[Cycle];
			if (Cycle>8) mirror=true;
			break;
		case 16:
			c=Cycle;
			break;
		}
		Animation* a = af->GetCycle( c );
		anims[Cycle] = a;
		if (!a) continue;
		//animations are started at a random frame position
		//Always start from 0, unless set otherwise
		if (!(ExtFlags&PEF_RANDOM)) {
			a->SetPos(0);
		}

		if (mirror) {
			a->MirrorAnimation();
		}
		if (mirrorvert) {
			a->MirrorAnimationVert();
		}
		a->gameAnimation = true;
	}
}

//apply gradient colors
void Projectile::SetupPalette(Animation *anim[], Palette *&pal, const ieByte *gradients)
{
	ieDword Colors[7];

	for (int i=0;i<7;i++) {
		Colors[i]=gradients[i];
	}
	GetPaletteCopy(anim, pal);
	if (pal) {
		pal->SetupPaperdollColours(Colors, 0);
	}
}

void Projectile::GetPaletteCopy(Animation *anim[], Palette *&pal)
{
	if (pal)
		return;
	for (unsigned int i=0;i<MAX_ORIENT;i++) {
		if (anim[i]) {
			Sprite2D* spr = anim[i]->GetFrame(0);
			if (spr) {
				pal = core->GetVideoDriver()->GetPalette(spr)->Copy();
			}
		}
	}
}

void Projectile::SetBlend()
{
	GetPaletteCopy(travel, palette);
	if (!palette)
		return;
	if (!palette->alpha) {
		palette->CreateShadedAlphaChannel();
	}
}

//create another projectile with type-1 (iterate magic missiles and call lightning)
void Projectile::CreateIteration()
{
	ProjectileServer *server = core->GetProjectileServer();
	Projectile *pro = server->GetProjectileByIndex(type-1);
	pro->SetEffectsCopy(effects);
	pro->SetCaster(Caster);
	area->AddProjectile(pro, Pos, Target);
}

// load animations, start sound
void Projectile::Setup()
{
	tint.r=128;
	tint.g=128;
	tint.b=128;
	tint.a=255;

	ieDword time = core->GetGame()->Ticks;
	timeStartStep = time;

	if(ExtFlags&(PEF_FALLING|PEF_INCOMING) ) {
		if (ExtFlags&PEF_FALLING) {
			Pos.x=Destination.x;
		} else {
			Pos.x=Destination.x+200;
		}
		Pos.y=Destination.y-200;
		NextTarget(Destination);
	}

	if(ExtFlags&PEF_WALL) {
		SetupWall();
	}

	//cone area of effect always disables the travel flag
	//but also makes the caster immune to the effect
	if (Extension && (Extension->AFlags&PAF_CONE)) {
		Destination=Pos;
		ExtFlags|=PEF_NO_TRAVEL;
	}

	//set any static tint
	if(ExtFlags&PEF_TINT) {
		Color tmpColor[PALSIZE];

		core->GetPalette( Gradients[0], PALSIZE, tmpColor );
		StaticTint(tmpColor[PALSIZE/2]);
	}

	core->GetAudioDrv()->Play(SoundRes1, Pos.x, Pos.y, GEM_SND_RELATIVE);
	memset(travel,0,sizeof(travel));
	memset(shadow,0,sizeof(shadow));
	light = NULL;

	CreateAnimations(travel, BAMRes1, Seq1);

	if (TFlags&PTF_SHADOW) {
		CreateAnimations(shadow, BAMRes2, Seq2);
	}

	//there is no travel phase, create the projectile right at the target
	if (ExtFlags&PEF_NO_TRAVEL) {
		Pos = Destination;

		//the travel projectile should linger after explosion
		if(ExtFlags&PEF_POP) {
			//the explosion consists of a pop in/hold/pop out of the travel projectile (dimension door)
			if(travel[0] && shadow[0]) {
				SetDelay(travel[0]->GetFrameCount()*2+shadow[0]->GetFrameCount() );
				travel[0]->Flags|=A_ANI_PLAYONCE;
				shadow[0]->Flags|=A_ANI_PLAYONCE;
			}
		} else {
			if(travel[0]) {
				SetDelay(travel[0]->GetFrameCount() );
			}
		}
	}

	if (TFlags&PTF_COLOUR) {
		SetupPalette(travel, palette, Gradients);
	} else {
		gamedata->FreePalette(palette, PaletteRes);
		palette=gamedata->GetPalette(PaletteRes);
	}

	if (TFlags&PTF_LIGHT) {
		light = core->GetVideoDriver()->CreateLight(LightX, LightZ);
	}
	if (TFlags&PTF_BLEND) {
		SetBlend();
	}
	phase = P_TRAVEL;

	//create more projectiles
	if(ExtFlags&PEF_ITERATION) {
		CreateIteration();
	}
}

Actor *Projectile::GetTarget()
{
	Actor *target;

	if (Target) {
		target = area->GetActorByGlobalID(Target);
		if (!target) return NULL;
		Actor *original = area->GetActorByGlobalID(Caster);
		if (original==target) {
			effects->SetOwner(target);
			return target;
		}
		int res = effects->CheckImmunity ( target );
		//resisted
		if (!res) {
			return NULL;
		}
		if (res==-1) {
			Target = original->GetID();
			return NULL;
		}
		effects->SetOwner(original);
		return target;
	}
	target = area->GetActorByGlobalID(Caster);
	if (target) {
		effects->SetOwner(target);
	}
	return target;
}

void Projectile::SetDelay(int delay)
{
	extension_delay=delay;
	ExtFlags|=PEF_FREEZE;
}

void Projectile::Payload()
{
	Actor *target;

	if(!effects) {
		return;
	}

	if (Target) {
		target = GetTarget();
		if (!target && (Target==Caster)) {
			//projectile rebounced
			return;
		}
	} else {
		//the target will be the original caster
		//in case of single point area target (dimension door)
		target = area->GetActorByGlobalID(Caster);
		if (target) {
			effects->SetOwner(target);
		}
	}
	if (target) {
		effects->AddAllEffects(target, Destination);
	}
	delete effects;
	effects = NULL;
}

//control the phase change when the projectile reached its target
//possible actions: vanish, hover over point, explode
//depends on the area extension
//play explosion sound
void Projectile::ChangePhase()
{
	if (Target) {
		Actor *target = area->GetActorByGlobalID(Target);
		if (!target) {
			phase = P_EXPIRED;
			return;
		}
	}
	//reached target, and explodes now
	if (!Extension) {
		//there are no-effect projectiles, like missed arrows
		//Payload can redirect the projectile in case of projectile reflection
		if (effects) {
			Payload();
		}
		//freeze on target, this is recommended only for child projectiles
		//as the projectile won't go away on its own
		if(ExtFlags&PEF_FREEZE) {
			if(extension_delay) {
				if (extension_delay>0) {
					extension_delay--;
				}
				return;
			}
		}
		if(ExtFlags&PEF_FADE) {
			TFlags &= ~PTF_TINT; //turn off area tint
			tint.a--;
			if(tint.a>0) {
				return;
			}
		}

		phase = P_EXPIRED;
		return;
	}

	EndTravel();
}

void Projectile::EndTravel()
{
	if(!Extension) {
		phase = P_EXPIRED;
		return;
	}

	//this flag says the first explosion is delayed (works for delaying triggers too)
	if(Extension->AFlags&PAF_DELAY) {
		extension_delay=Extension->Delay;
	} else {
		extension_delay=0;
	}
	extension_explosioncount=Extension->ExplosionCount;

	//this flag says that the explosion should occur only when triggered
	if (Extension->AFlags&PAF_TRIGGER) {
		phase = P_TRIGGER;
	} else {
		phase = P_EXPLODING1;
	}
}

void Projectile::DoStep(unsigned int walk_speed)
{
	if (!path) {
		ChangePhase();
		return;
	}

	if (Pos==Destination) {
		ClearPath();
		ChangePhase();
		return;
	}

	if (ExtFlags&PEF_LINE) {
		if(Extension) {
			//transform into an explosive line
			EndTravel();
		} else {
			if(!(ExtFlags&PEF_FREEZE) && travel[0]) {
				//switch to 'fading' phase
				//SetDelay(travel[0]->GetFrameCount());
				SetDelay(100);
			}
			ChangePhase();
		}
		//don't change position
		return;
	}

	//path won't be calculated if speed==0
	walk_speed=1500/walk_speed;
	ieDword time = core->GetGame()->Ticks;
	if (!step) {
		step = path;
		//timeStartStep = time;
	}
	while (step->Next && (( time - timeStartStep ) >= walk_speed)) {
		step = step->Next;
		if (!walk_speed) {
			timeStartStep = time;
			break;
		}
		timeStartStep = timeStartStep + walk_speed;
	}

	SetOrientation (step->orient, false);

	Pos.x=step->x;
	Pos.y=step->y;
	if (!step->Next) {
		ClearPath();
		NewOrientation = Orientation;
		ChangePhase();
		return;
	}
	if (!walk_speed) {
		return;
	}
	if (step->Next->x > step->x)
		Pos.x += ( unsigned short )
			( ( step->Next->x - Pos.x ) * ( time - timeStartStep ) / walk_speed );
	else
		Pos.x -= ( unsigned short )
			( ( Pos.x - step->Next->x ) * ( time - timeStartStep ) / walk_speed );
	if (step->Next->y > step->y)
		Pos.y += ( unsigned short )
			( ( step->Next->y - Pos.y ) * ( time - timeStartStep ) / walk_speed );
	else
		Pos.y -= ( unsigned short )
			( ( Pos.y - step->Next->y ) * ( time - timeStartStep ) / walk_speed );
}

void Projectile::SetCaster(ieDword caster)
{
	Caster=caster;
}

ieDword Projectile::GetCaster()
{
	return Caster;
}

void Projectile::NextTarget(Point &p)
{
	ClearPath();
	Destination = p;
	//call this with destination
	if (path) {
		return;
	}
	if (!Speed) {
		Pos = Destination;
		return;
	}
	NewOrientation = Orientation = GetOrient(Destination, Pos);

	//this hack ensures that the projectile will go away after its time
	//by the time it reaches this part, it was already expired, so Target
	//needs to be cleared.
	if(ExtFlags&PEF_NO_TRAVEL) {
		Target = 0;
		Destination = Pos;
		return;
	}
	path = area->GetLine( Pos, Destination, Speed, Orientation, GL_PASS );
}

void Projectile::SetTarget(Point &p)
{
	Target = 0;
	NextTarget(p);
}

void Projectile::SetTarget(ieDword tar)
{
	Target = tar;
	Actor *target = area->GetActorByGlobalID(tar);
	if (!target) {
		phase = P_EXPIRED;
		return;
	}
	//replan the path in case the target moved
	if(target->Pos!=Destination) {
		NextTarget(target->Pos);
		return;
	}

	//replan the path in case the source moved (only for line projectiles)
	if(ExtFlags&PEF_LINE) {
		Actor *c = area->GetActorByGlobalID(Caster);
		if(c && c->Pos!=Pos) {
			Pos=c->Pos;
			NextTarget(target->Pos);
		}
	}
}

void Projectile::MoveTo(Map *map, Point &Des)
{
	area = map;
	Pos = Des;
	Destination = Des;
}

void Projectile::ClearPath()
{
	PathNode* thisNode = path;
	while (thisNode) {
		PathNode* nextNode = thisNode->Next;
		delete( thisNode );
		thisNode = nextNode;
	}
	path = NULL;
	step = NULL;
}

int Projectile::CalculateTargetFlag()
{
	//if there are any, then change phase to exploding
	int flags = GA_NO_DEAD;

	//projectiles don't affect dead/inanimate normally
	if (Extension->AFlags&PAF_INANIMATE) {
		flags&=~GA_NO_DEAD;
	}

	//affect only enemies or allies
	switch (Extension->AFlags&PAF_TARGET) {
	case PAF_ENEMY:
		flags|=GA_NO_ALLY;
		break;
	case PAF_PARTY:
		flags|=GA_NO_ENEMY;
		break;
	}
	return flags;
}

//get actors covered in area of trigger radius
void Projectile::CheckTrigger(unsigned int radius)
{
	if (area->GetActorInRadius(Pos, CalculateTargetFlag(), radius)) {
		if (phase == P_TRIGGER) {
			phase = P_EXPLODING1;
			extension_delay = Extension->Delay;
		}
	} else {
		if (phase == P_EXPLODING1) {
			//the explosion is revoked
			if (Extension->AFlags&PAF_SYNC) {
				phase = P_TRIGGER;
			}
		}
	}
}

void Projectile::SetEffectsCopy(EffectQueue *eq)
{
	if(effects) delete effects;
	if(!eq) {
		effects=NULL;
		return;
	}
	effects = eq->CopySelf();
}

void Projectile::UpdateLine()
{
	if (Target) {
		SetTarget(Target);
	}
}

void Projectile::LineTarget()
{
	Actor *original = area->GetActorByGlobalID(Caster);
	Actor *prev = NULL;
	PathNode *iter = path;
	while(iter) {
		Point pos(iter->x,iter->y);
		Actor *target = area->GetActorInRadius(pos, CalculateTargetFlag(), 1);
		if (target && target->GetGlobalID()!=Caster && prev!=target) {
			prev = target;
	 		int res = effects->CheckImmunity ( target );
			if (res>0) {
				EffectQueue *eff = effects->CopySelf();
				eff->SetOwner(original);
				effects->AddAllEffects(target, target->Pos);
			}
		}
		iter = iter->Next;
	}
}

//secondary projectiles target all in the explosion radius
void Projectile::SecondaryTarget()
{
	int mindeg = 0;
	int maxdeg = 0;

	//the AOE (area of effect) is cone shaped
	if (Extension->AFlags&PAF_CONE) {
		mindeg=(Orientation*45-Extension->ConeWidth)/2;
		maxdeg=mindeg+Extension->ConeWidth;
	}

	ProjectileServer *server = core->GetProjectileServer();
	int radius = Extension->ExplosionRadius;
	Actor **actors = area->GetAllActorsInRadius(Pos, CalculateTargetFlag(), radius);
	Actor **poi=actors;
	while(*poi) {
		ieDword Target = (*poi)->GetGlobalID();

		//this flag is actually about ignoring the caster (who is at the center)
		if ((SFlags & PSF_IGNORE_CENTER) && (Caster==Target)) {
			poi++;
			continue;
		}

		if (Extension->AFlags&PAF_CONE) {
			//cone never affects the caster
			if(Caster==Target) {
				poi++;
				continue;
			}
			double xdiff = Pos.x-(*poi)->Pos.x;
			double ydiff = Pos.y-(*poi)->Pos.y;
			int deg;

			if (ydiff) {
				deg = (int) (atan(xdiff/ydiff)*180/M_PI);
				if(ydiff<0) deg+=180;
			} else {
				if (xdiff<0) deg=0;
				else deg = 180;
			}

			//not in the right sector of circle
			if (mindeg>deg || maxdeg<deg) {
				poi++;
				continue;
			}
		}
		Projectile *pro = server->GetProjectileByIndex(Extension->ExplProjIdx);
		pro->SetEffectsCopy(effects);
		pro->SetCaster(Caster);
		area->AddProjectile(pro, Pos, Target);
		poi++;

		//we already got one target affected in the AOE, this flag says
		//that was enough
		if(Extension->AFlags&PAF_AFFECT_ONE) {
			break;
		}
	}
	free(actors);
}

int Projectile::Update()
{
	//if reached target explode
	//if target doesn't exist expire
	if (phase == P_EXPIRED) {
		return 0;
	}
	if (phase == P_UNINITED) {
		Setup();
	}

	int pause = core->IsFreezed();
	if (pause) {
		return 1;
	}
	//recreate path if target has moved
	if(Target) {
		SetTarget(Target);
	}

	if (phase == P_TRAVEL) {
		DoStep(Speed);
	}
	return 1;
}

void Projectile::Draw(Region &screen)
{
	switch (phase) {
		case P_UNINITED:
			return;
		case P_TRIGGER: case P_EXPLODING1:case P_EXPLODING2:
			//This extension flag is to enable the travel projectile at
			//trigger/explosion time
			if (Extension->AFlags&PAF_VISIBLE) {
				DrawTravel(screen);
			}
			CheckTrigger(Extension->TriggerRadius);
			if (phase == P_EXPLODING1 || phase == P_EXPLODING2) {
				DrawExplosion(screen);
			}
			break;
		case P_TRAVEL:
			//There is no Extension for simple traveling projectiles!
			DrawTravel(screen);
			return;
		default:
			DrawExploded(screen);
			return;
	}
}

bool Projectile::DrawChildren(Region &screen)
{
	bool drawn = false;

	if (children) {
		for(int i=0;i<child_size;i++){
			if(children[i]) {
				if (children[i]->Update()) {
					children[i]->DrawTravel(screen);
					drawn = true;
				} else {
					delete children[i];
					children[i]=NULL;
				}
			}
		}
	}

	return drawn;
}

//draw until all children expire
void Projectile::DrawExploded(Region &screen)
{
	if (DrawChildren(screen)) {
		return;
	}
	phase = P_EXPIRED;
}

void Projectile::DrawExplosion(Region &screen)
{
	//This seems to be a needless safeguard
	if (!Extension) {
		phase = P_EXPIRED;
		return;
	}

	DrawChildren(screen);

	int pause = core->IsFreezed();
	if (pause) {
		return;
	}

	//Delay explosion, it could even be revoked with PAF_SYNC (see skull trap)
	if (extension_delay) {
		extension_delay--;
		return;
	}

	//0 and 1 have the same effect (1 explosion)
	if (extension_explosioncount) {
		extension_explosioncount--;
	}

	//play VVC in center
	if (Extension->AFlags&PAF_VVC) {
		ScriptedAnimation* vvc = gamedata->GetScriptedAnimation(Extension->VVCRes, false);
		if (vvc) {
			vvc->XPos+=Pos.x;
			vvc->YPos+=Pos.y;
			area->AddVVCell(vvc);
		}
	}

	//Line targets are actors between source and destination point
	if(ExtFlags&PEF_LINE) {
		UpdateLine();
		LineTarget();
	}

	//no idea what is PAF_SECONDARY
	//probably it is to alter some behaviour in the secondary
	//projectile generation
	//In trapskul.pro it isn't set, yet it has a secondary (invisible) projectile
	//All area effects are created by secondary projectiles

	//the secondary projectile will target everyone in the area of effect
	SecondaryTarget();

	//draw fragment graphics animation at the explosion center
	if (Extension->AFlags&PAF_FRAGMENT) {
		//there is a character animation in the center of the explosion
		//which will go towards the edges (flames, etc)
		//Extension->ExplColor fake color for single shades (blue,green,red flames)
		//Extension->FragAnimID the animation id for the character animation
		//This color is not used in the original game
		area->Sparkle(Extension->ExplColor, SPARKLE_EXPLOSION, Pos, Extension->FragAnimID);
	}

	ProjectileServer *server = core->GetProjectileServer();
	//the center of the explosion could be another projectile played over the target
	if (Extension->FragProjIdx) {
		Projectile *pro = server->GetProjectileByIndex(Extension->FragProjIdx);
		if (pro) {
			area->AddProjectile(pro, Pos, Pos);
		}
	}

	//the center of the explosion is based on hardcoded explosion type (this is fireball.cpp in the original engine)
	//these resources are listed in areapro.2da and served by ProjectileServer.cpp
	if (Extension->ExplType!=0xff) {
		ieResRef const *res;
		int apflags = server->GetExplosionFlags(Extension->ExplType);

		//draw it only once, at the time of explosion
		if (phase==P_EXPLODING1) {
			res = server->GetExplosion(Extension->ExplType, 3);
			if (res) {
				core->GetAudioDrv()->Play(*res, Pos.x, Pos.y, GEM_SND_RELATIVE);
			}
			//the center animation is in the second column
			res = server->GetExplosion(Extension->ExplType, 1);
			if (res) {
				ScriptedAnimation* vvc = gamedata->GetScriptedAnimation(*res, false);
				if (vvc) {
					if (apflags & APF_VVCPAL) {
						vvc->SetPalette(Extension->ExplColor);
					}
					vvc->XPos+=Pos.x;
					vvc->YPos+=Pos.y;
					vvc->PlayOnce();
					area->AddVVCell(vvc);
				}
			}
			phase=P_EXPLODING2;
		} else {
			res = server->GetExplosion(Extension->ExplType, 4);
			if (res) {
				core->GetAudioDrv()->Play(*res, Pos.x, Pos.y, GEM_SND_RELATIVE);
			}
		}

		//the spreading animation is in the first column
		res = server->GetExplosion(Extension->ExplType, 0);
		//returns if the explosion animation is fake coloured
		if (res) {
			if (!children) {
				child_size=(Extension->ExplosionRadius+15)/16;
				//more sprites if the whole area needs to be filled
				if (apflags&APF_FILL) child_size*=2;
				if (apflags&APF_SPREAD) child_size*=2;
				children = (Projectile **) calloc(sizeof(Projectile *), child_size);
			}

			int initial = child_size;

			for(int i=0;i<initial;i++) {
				//leave this slot free, it is residue from the previous flare up
				if (children[i])
					continue;
				//create a custom projectile with single traveling effect
				Projectile *pro = server->CreateDefaultProjectile((unsigned int) ~0);
				strnlwrcpy(pro->BAMRes1, *res, 8);
				pro->SetEffects(NULL);
				pro->Speed=Speed;
				//calculate the child projectile's target point, it is either
				//a perimeter or an inside point of the explosion radius
				int rad = Extension->ExplosionRadius;
				Point newdest;

				if (apflags&APF_FILL) {
					rad=core->Roll(1,rad,0);
				}
				int max = 360;
				int add = 0;
				if (Extension->AFlags&PAF_CONE) {
					max=Extension->ConeWidth;
					add=(Orientation*45-max)/2;
				}
				max=core->Roll(1,max,add);
				double degree=max*M_PI/180;
				newdest.x = (int) -(rad * sin(degree) );
				newdest.y = (int) (rad * cos(degree) );

				if (apflags&APF_FILL) {
					pro->SetDelay(Extension->Delay*extension_explosioncount);
				}

				newdest.x+=Destination.x;
				newdest.y+=Destination.y;

				if (apflags&APF_SCATTER) {
					pro->MoveTo(area, newdest);
				} else {
					pro->MoveTo(area, Pos);
				}
				pro->SetTarget(newdest);
				pro->autofree=true;

				//sets up the gradient color for the explosion animation
				if (apflags&(APF_PALETTE|APF_TINT) ) {
					pro->SetGradient(Extension->ExplColor, !(apflags&APF_PALETTE));
				}
				//i'm unsure if we need blending for all anims or just the tinted ones
				pro->TFlags|=PTF_BLEND;
				//random frame is needed only for some of these, make it an areapro flag?
				pro->ExtFlags|=PEF_RANDOM;
				pro->Setup();
				children[i]=pro;
			}
		}
	}

	if (extension_explosioncount) {
		extension_delay=Extension->Delay;
	} else {
		phase = P_EXPLODED;
	}
}

int Projectile::GetTravelPos(int face)
{
	if (travel[face]) {
		return travel[face]->GetCurrentFrame();
	}
	return 0;
}

int Projectile::GetShadowPos(int face)
{
	if (shadow[face]) {
		return shadow[face]->GetCurrentFrame();
	}
	return 0;
}

void Projectile::SetPos(int face, int frame1, int frame2)
{
	if (travel[face]) {
		travel[face]->SetPos(frame1);
	}
	if (shadow[face]) {
		shadow[face]->SetPos(frame2);
	}
}

//recalculate target and source points (perpendicular bisector)
void Projectile::SetupWall()
{
	Point p1, p2;

	p1.x=(Pos.x+Destination.x)/2;
	p1.y=(Pos.y+Destination.y)/2;

	p2.x=p1.x+(Pos.y-Destination.y);
	p2.y=p1.y+(Pos.x-Destination.x);
	Pos=p1;
	SetTarget(p2);
}

void Projectile::DrawLine(Region &screen, int face, ieDword flag)
{
 	Video *video = core->GetVideoDriver();
	PathNode *iter = path;
	Sprite2D *frame = travel[face]->NextFrame();
	while(iter) {
		Point pos(iter->x, iter->y);

		if (SFlags&PSF_FLYING) {
			pos.y-=FLY_HEIGHT;
		}
		pos.x+=screen.x;
		pos.y+=screen.y;

		video->BlitGameSprite( frame, pos.x, pos.y, flag, tint, NULL, palette, &screen);
		iter = iter->Next;
	}
}

void Projectile::DrawTravel(Region &screen)
{
	Video *video = core->GetVideoDriver();
	ieDword flag;

	if(ExtFlags&PEF_HALFTRANS) {
		flag=BLIT_HALFTRANS;
	} else {
		flag = 0;
	}

	//static tint (use the tint field)
	if(ExtFlags&PEF_TINT) {
		flag|=BLIT_TINTED;
	}

	//Area tint
	if (TFlags&PTF_TINT) {
		tint = area->LightMap->GetPixel( Pos.x / 16, Pos.y / 12);
		flag |= BLIT_TINTED;
	}

	unsigned int face = GetNextFace();
	if (face!=Orientation) {
		SetPos(face, GetTravelPos(face), GetShadowPos(face));
	}
	Point pos = Pos;
	pos.x+=screen.x;
	pos.y+=screen.y;

	if (light) {
		video->BlitGameSprite( light, pos.x, pos.y, 0, tint, NULL, NULL, &screen);
	}

	if (ExtFlags&PEF_POP) {
			//draw pop in/hold/pop out animation sequences
			Sprite2D *frame;
			
			if(ExtFlags&PEF_UNPOP) {
				frame = shadow[0]->NextFrame();
				if(shadow[0]->endReached) {
					ExtFlags&=~PEF_UNPOP;
				}
			} else {
				frame = travel[0]->NextFrame();
				if(travel[0]->endReached) {
					travel[0]->playReversed=true;
					travel[0]->SetPos(0);
					ExtFlags|=PEF_UNPOP;
					frame = shadow[0]->NextFrame();
				}
			}
			
			video->BlitGameSprite( frame, pos.x, pos.y, flag, tint, NULL, palette, &screen);
			return;
	}
	
	if (ExtFlags&PEF_LINE) {
		DrawLine(screen, face, flag);
		return;
	}
	
	if (shadow[face]) {
		Sprite2D *frame = shadow[face]->NextFrame();
		video->BlitGameSprite( frame, pos.x, pos.y, flag, tint, NULL, palette, &screen);
	}

	if (SFlags&PSF_FLYING) {
		pos.y-=FLY_HEIGHT;
	}

	if (ExtFlags&PEF_PILLAR) {
		//draw all frames simultaneously on top of each other
		for(int i=0;i<Aim;i++) {
			if (travel[i]) {
				Sprite2D *frame = travel[i]->NextFrame();
				video->BlitGameSprite( frame, pos.x, pos.y, flag, tint, NULL, palette, &screen);
				pos.y-=frame->YPos;
			}
		}
	} else {
		if (travel[face]) {
			Sprite2D *frame = travel[face]->NextFrame();
			video->BlitGameSprite( frame, pos.x, pos.y, flag, tint, NULL, palette, &screen);
		}
	}

	if (SFlags&PSF_SPARKS) {
		area->Sparkle(SparkColor,SPARKLE_PUFF,pos);
	}
}

void Projectile::SetIdentifiers(const char *resref, ieWord id)
{
	strnuprcpy(name, resref, sizeof(name));
	type=id;
}

bool Projectile::PointInRadius(Point &p)
{
	switch(phase) {
		//better not trigger on projectiles unset/expired
		case P_EXPIRED:
		case P_UNINITED: return false;
		case P_TRAVEL:
			if(p.x==Pos.x && p.y==Pos.y) return true;
			return false;
		default:
			if(p.x==Pos.x && p.y==Pos.y) return true;
			if (!Extension) return false;
			if (Distance(p,Pos)<Extension->ExplosionRadius) return true;
	}
	return false;
}

//Set gradient color, if type is true then it is static tint, otherwise it is paletted color
void Projectile::SetGradient(int gradient, bool type)
{
	//gradients are unsigned chars, so this works
	memset(Gradients, gradient, 7);
	if (type) {
		ExtFlags|=PEF_TINT;
	} else {
		TFlags |= PTF_COLOUR;
	}
}

void Projectile::StaticTint(Color &newtint)
{
	tint = newtint;
	TFlags &= ~PTF_TINT; //turn off area tint
}

void Projectile::Cleanup()
{
	//neutralise the payload
	delete effects;
	effects = NULL;
	//diffuse the projectile
	phase=P_EXPIRED;
}

