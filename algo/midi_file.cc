// algo/midi_file.cc

// Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>
// All rights reserved.
//
// This file is part of wx12Ton, http://wx12ton.sourceforge.net/
//
// wx12Ton is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2, or (at your option) any later version.
// 
// wx12Ton is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// wx12Ton; see the file COPYING or COPYING.txt. If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

#include "midi_file.h"
//std::copy
#include <algorithm>

//MH stands for "MIDI helper"
static const uint8_t cpMH_Header[18]={
 'M','T','h','d',0x00,0x00,0x00,0x06,//header haeder
 0x00,0x00,0x00,0x01,                //midi type 0,i.e. single track
 0x01,0x00,                          //256 ticks per quarter
 'M','T','r','k'                     //track 1 haeder
};
static const uint8_t cpMH_CommentHeader[3]={
 0x00,0xff,0x01
};
static const uint8_t cpMH_InitTrack[8]={
 0x00,0xff,0x58,0x04,0x01,0x02,0x18,0x08 // 1/4 measure
};
static const uint8_t cpMH_EndTrack[4]={
 0x00,0xff,0x2f,0x00
};

midi_ofile::cvec midi_ofile::m_int(uint32_t _i){
 cvec v(4);
 v[0]=(_i&0xff000000)>>24;
 v[1]=(_i&0x00ff0000)>>16;
 v[2]=(_i&0x0000ff00)>> 8;
 v[3]=(_i&0x000000ff)>> 0;
 return v;
}

midi_ofile::cvec midi_ofile::m_vint(uint32_t _i){
 cvec v;
 uint32_t i=_i&0x7f;
 while((_i>>=7)>0){i<<=8;i|=0x80;i+=(_i&0x7f);}
 for(;;){
  v.push_back(i);
  if(i&0x80)i>>=8;else break;
 }
 return v;
}

void midi_ofile::set_comment(const std::string& _s){
 if(_s.empty()){m_vComment.clear();m_iCommentSize=0;return;}

 m_vComment=m_vint(_s.length());
 size_t iS=m_vComment.size();
 m_iCommentSize=iS+_s.size();
 m_vComment.resize(m_iCommentSize);

 std::copy(_s.begin(),_s.end(),m_vComment.begin()+iS);
}

void midi_ofile::append_note(uint8_t _c,uint32_t _iLength,uint8_t _cLoud){
 m_pn(0x00);
 if(m_iNotesSize==1)m_pn(0x90);
 m_pn(_c);
 m_pn(_cLoud);
 
 const cvec v=m_vint(_iLength);
 for(cvec::const_iterator i=v.begin();i!=v.end();++i)m_pn(*i);

 m_pn(_c);
 m_pn(0x00);
}

uint8_t midi_ofile::at(size_t _i)const{
 if(_i<18)return cpMH_Header[_i];
 if(_i<22)return m_int(size()-22)[_i-18];
 if(_i<25)return cpMH_CommentHeader[_i-22];
 if(_i<25+m_iCommentSize)return m_vComment[_i-25];
 if(_i<25+m_iCommentSize+8)return cpMH_InitTrack[_i-25-m_iCommentSize];
 if(_i<25+m_iCommentSize+8+m_iNotesSize)return m_vNotes[_i-25-m_iCommentSize-8];
 return cpMH_EndTrack[_i-25-m_iCommentSize-8-m_iNotesSize];
}

static const std::string sMIDI_SeriesStart=">>";
static const std::string sMIDI_Header=
"This is a !12Ton MIDI file"
#ifdef PACKAGE_STRING
 ", created by " PACKAGE_STRING
#endif
 ".";
static const int iMIDI_ContentLen=128;

bool Series_IO::write(const Series& _s,uint8_t _cLoud){
 midi_ofile mof;
 std::string s;
 for(int i=0;i<12;++i){
  if(_s.at(i).no_oct())s+='a';
  else s+='b'+_s.at(i).get_oct()-Note::OCT_MIN;
  s+='A'+_s.at(i).get_val()-Note::TONE_MIN;
 }
 mof.set_comment(sMIDI_Header+sMIDI_SeriesStart+s);
 for(int i=0;i<12;++i){
  int n=_s.at(i).get_val()
   +((!_s.at(i).no_oct())?_s.at(i).get_oct()*12:0)
   +int(midi_ofile::MIDI_C1);
  int j=0;
  if(n<midi_ofile::MIDI_LOW_NOTE ){
   j=(midi_ofile::MIDI_LOW_NOTE-n)/12+1;n=n+j*12;}
  if(n>midi_ofile::MIDI_HIGH_NOTE){
   j=(n-midi_ofile::MIDI_HIGH_NOTE)/12+1;n=n-j*12;}
  for(int k=0;k<=j;++k)mof.append_note(n,midi_ofile::MIDI_120_BPM/(1<<j),
                                       _cLoud);
 }

 for(size_t i=0;i<mof.size();++i)if(!write_char(char(mof.at(i))))return false;
 
 return true;
}

std::pair<bool,Series> Series_IO::read(){
 char cp[iMIDI_ContentLen];
 ssize_t iR=read_block(cp,iMIDI_ContentLen);
 if(iR<0)return std::make_pair(false,Series());

 char* cpB=std::find_end(cp,cp+iR,
  sMIDI_SeriesStart.begin(),sMIDI_SeriesStart.end());
 cpB+=sMIDI_SeriesStart.length();
 if(cpB+24-cp>iR)return std::make_pair(false,Series());

 Series S;
 for(int i=0;i<12;++i){
  if(cpB[i*2+1]<'A' || cpB[i*2+1]>='A'+12 ||
     cpB[i*2  ]<'a' || cpB[i*2  ]>='a'+16)return std::make_pair(false,Series());
  S.at(i).set_val(cpB[i*2+1]-'A'+Note::TONE_MIN);
  if(cpB[i*2]=='a')S.at(i).set_no_oct();
  else S.at(i).set_oct(cpB[i*2]-'b'+Note::OCT_MIN);
 }

 return std::make_pair(true,S);
}

