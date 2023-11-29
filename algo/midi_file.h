/// \file algo/midi_file.h
/// \author Matthias Kleinmann \<Matthias dot Kleinmann at gmx dot de\>
/// \brief write series to midi file

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

#ifndef MIDI_FILE_H
#define MIDI_FILE_H

//uint8_t,uint32_t
#include <stdint.h>
//std::pair
#include <utility>
#include <vector>
#include <string>

#include "series.h"

class midi_ofile{
 public:
  enum {MIDI_120_BPM=256,MIDI_DEF_LOUD=0x40,MIDI_C1=0x3c,MIDI_LOW_NOTE=0x00,
        MIDI_HIGH_NOTE=0x7f};

  midi_ofile():m_iCommentSize(0),m_iNotesSize(0){}
  void set_comment(const std::string& _s);
  void append_note(uint8_t,uint32_t=MIDI_120_BPM,uint8_t=MIDI_DEF_LOUD);

  size_t size()const{return 25+m_iCommentSize+8+m_iNotesSize+4;}
  uint8_t at(size_t _i)const;

 private:
  typedef std::vector<uint8_t> cvec;

  static cvec m_int(uint32_t);
  static cvec m_vint(uint32_t);

  void m_pn(uint8_t _c){m_vNotes.push_back(_c);++m_iNotesSize;}

  size_t m_iCommentSize,m_iNotesSize;
  cvec m_vComment,m_vNotes;
};

/*! \brief Read/Write #Series to a !12Ton MIDI-file.

  The file format is a midi file, which contains the series as 1/4 notes in a
  1/4 measure. A note value of #Note::TONE_MIN sounds as "c1". If no octave
  information is available, the the notes is this octave. If the octave is
  beyond the range of tones, which are allowed in a MIDI file, the note is
  repeated, encoding the missing octaves: E.g. if the note is in the lowest
  possible octave and is written as 2*1/8, one octave below this level is meant
  (analogous e.g. 8*1/32 means three octaves below).

  What is special about the !12Ton MIDI format is, that in the MIDI comment
  block the series is encoded as a string of length 24 (cf. 
  #Series::Series(const std::string&). This information is used to read out
  a !12Ton MIDI file.

  You have to dervive from this class and overload the methods
  #write_char(char)throw() and/or #read_block(char*,ssize_t)throw()
  in order to implement reading and/or writing to a file. By default, this
  methods return an error.
*/
class Series_IO{
 public:
  enum { /// loudness value, that represents an intermediate loudness
        MIDI_DEF_LOUD=midi_ofile::MIDI_DEF_LOUD
       };

  virtual ~Series_IO(){}

  /*! \brief read file

      \return The \c bool is \c true if reading was successfull.
  */
  std::pair<bool,Series> read();
  /// write series \p _s, with loundness _iLoud
  bool write(const Series& _s,uint8_t _iLoud=MIDI_DEF_LOUD);

 protected:
  /*! \brief write a char

      \retval true if success

      Overload this method (use e.g. fwrite() and convert retval) if you
      need file output. The default is to return an error.
  */
  virtual bool write_char(char)throw(){return false;}
  /*! \brief read a block of chars
      
      \param _cp pointer to data
      \param _iS length of data

      \return number of chars, successfully read, negative if error.
 
      Overload this method (use e.g. fwrite() and convert retval) if you
      need file input. The default is to return an error.

      \sa Even if the retval is smaller as \p _iS, no retry will be done.
          So retry in this function, if neccessary.
  */
  virtual ssize_t read_block(char* _cp,ssize_t _iS)throw();
};

inline ssize_t Series_IO::read_block(char*,ssize_t)throw(){return -1;}

#endif
