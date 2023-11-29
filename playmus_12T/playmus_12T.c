/* This is a modified version of the playmus application, that comes with the
   SDL mixer library.
   Changes by Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>
 
   Original playmus copyright:
   Copyright (C) 1997, 1998, 1999, 2000, 2001 Sam Lantinga <slouken@libsdl.org>
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_mixer.h"

static int audio_open=0;
static Mix_Music* music=NULL;

void CleanUp(){
 if(Mix_PlayingMusic())Mix_HaltMusic();
 if(music){Mix_FreeMusic(music);music=NULL;}
 if(audio_open){Mix_CloseAudio();audio_open=0;}
 SDL_Quit();
}

int main(int argc,char** argv){ /*SDL requires exactly this form of main()*/
 int audio_rate=22050;
 Uint16 audio_format=AUDIO_S16;
 int audio_channels=1;
 int audio_buffers=4096;
 const char* cpFile=NULL;

#define BEGIN_CASE(x,y) if(x){if(y){
#define END_CASE }else{\
 fputs("Usage: playmus_12T [-8] [-r rate] [-b buffers] -f <musicfile>\n",\
       stderr);return 1;}}
#define SWITCH {BEGIN_CASE(0,0) /*}*/
#define CASE1(x) END_CASE else BEGIN_CASE(!strcmp(*argv,(x)),1)
#define CASE2(x) END_CASE else BEGIN_CASE(!strcmp(*argv,(x)),*(argv+1)) ++argv;
#define END_SWITCH END_CASE else if(0){ END_CASE /*}*/
 if(argc) /*avoid unused argv  warning*/
 for(++argv;*argv;++argv)
  SWITCH
   CASE2("-f") cpFile        =*argv;
   CASE2("-r") audio_rate    =atoi(*argv);
   CASE2("-b") audio_buffers =atoi(*argv);
   CASE1("-m") audio_channels=1;
   CASE1("-8") audio_format  =AUDIO_U8;
  END_SWITCH

 BEGIN_CASE(1,cpFile)END_CASE

 if(SDL_Init(SDL_INIT_AUDIO)<0){
  fprintf(stderr, "playmus_12T: Couldn't initialize SDL: %s\n",SDL_GetError());
  return 2;
 }

 if(Mix_OpenAudio(audio_rate,audio_format,audio_channels,audio_buffers)<0){
  fprintf(stderr,"playmus_12T: Couldn't open audio: %s\n",SDL_GetError());
  CleanUp();
  return 2;
 }
 audio_open=1;
 Mix_VolumeMusic(MIX_MAX_VOLUME);

 music=Mix_LoadMUS(cpFile);
 if(!music){
  fprintf(stderr,"playmus_12T: Couldn't load %s: %s\n",cpFile,SDL_GetError());
  CleanUp();
  return 2;
 }
 
 Mix_PlayMusic(music,0);
 while(Mix_PlayingMusic())SDL_Delay(100);

 CleanUp();
 return 0;
}
