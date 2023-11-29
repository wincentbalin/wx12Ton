/// \file algo/series.h
/// \author Matthias Kleinmann \<Matthias dot Kleinmann at gmx dot de\>
/// \brief provides class #Note and class #Series

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

#ifndef SERIES_H
#define SERIES_H

#include <string>
//reverse,next_permutation,prev_permutation,lexicalographical_compare,equal
#include <algorithm>
//pair
#include <utility>
//uint8_t,int8_t
#include <stdint.h>

/*! \brief A single note with optional octave information.
    
    This class represents a single note. A note consists of two informations:
    \li the \em tone \em value, e.g. "c", "c sharp", ..., "b flat", "b". This
    value is represented as interval relative to a base note (in this case the
    tone "c", i.e. a tone value of #TONE_MIN corresponds to "c" and a value of
    #TONE_MAX corresponds to "b")
    \li the \em octave \em value (optional). E.g. an octave value of \c -1
    means one octave below the base note.
*/
class Note{
 public:
  enum {TONE_MIN=0, ///<minimal tone value, representing lowest tone
        TONE_MAX=11, ///<maximal tone value, representing highest tone
      
        OCT_MIN=-7, ///<minimal octave value
        OCT_MAX=7,  ///<maximal octave value

        ND_MIN=TONE_MIN+OCT_MIN*12,///<minimal interval allowed 
        ND_MAX=TONE_MAX+OCT_MAX*12 ///<maximal interval allowed 
       };
  typedef int NoteDiff; ///<type for intervals
  typedef uint8_t Tone; ///<type for tone value
  typedef int8_t Octave;///<type for octave value

  ///plain c'tor 
  /*! \note This c'tor does not initialize, and thus might hold an invalid
            note.
  */
  Note(){}

  ///Initialize note.
  /*! \param _nd  interval relative to base note
      \param _bNO flag, whether \em no octave value should present
  
      Sets the tone value and octave value to represent a note with
      interval \p _nd to the base note.
  */
  explicit Note(NoteDiff _nd,bool _bNO=false){m_set(_nd,_bNO);}
  /// shortcut to \code operator =(Note(_nd,_bNO)) \endcode
  void set(NoteDiff _nd,bool _bNO=false){m_set(_nd,_bNO);}

  ///get tone value
  Tone get_val()const{return m_iTone;}
  ///get octave value, undefined result if no octave information
  Octave get_oct()const{return m_iOctave;}
  ///test whether octave information is present
  bool no_oct()const{return get_oct()==OCT_NONE;}

  ///set tone value
  void set_val(Tone _t){m_iTone=_t;}
  ///set octave value
  void set_oct(Octave _o){m_iOctave=_o;}
  ///remove octave value
  void set_no_oct(){m_iOctave=OCT_NONE;}


  ///limit a #NoteDiff to be in the range [#TONE_MIN ... #TONE_MAX]
  static Tone modulo(NoteDiff _nd){return (_nd-(_nd/12)*12+12)%12;}

  /*! \name calculation support
  */
  //@{
  NoteDiff operator -(const Note& _n)const{return m_get()-_n.m_get();}

  Note operator -(const NoteDiff& _nd)const{
   return Note(get_val()+get_oct()*12-_nd,no_oct());}
  Note operator +(const NoteDiff& _nd)const{
   return Note(get_val()+get_oct()*12+_nd,no_oct());}

  Note& operator -=(const NoteDiff& _nd){
   m_set(get_val()+get_oct()*12-_nd,no_oct());return *this;}
  Note& operator +=(const NoteDiff& _nd){
   m_set(get_val()+get_oct()*12+_nd,no_oct());return *this;}
  //@}

  /*! \name comparing support
  */
  //@{
  ///compare tone value (ignores octave value)
  bool operator  <(const Note& _n)const{return get_val() <_n.get_val();}
  bool operator ==(const Note& _n)const{return get_val()==_n.get_val();}
  ///compare both, note and octave value
  /*! \note if a note without octave value is included, this can differ from
            \code (operator -(const Note& _n))==0 \endcode
  */
  bool full_eq(const Note& _n)const{
   return (*this)==_n && ((no_oct() && _n.no_oct()) || get_oct()==_n.get_oct());
  }
  //@}
 private:
  enum{OCT_NONE=-127};

  NoteDiff m_get()const{return get_val()+(no_oct()?0:get_oct())*12;}
  void m_set(NoteDiff _nd,bool _bNO){
    set_val(modulo(_nd));set_oct(_bNO?OCT_NONE:((_nd-get_val())/12));}

  int m_iTone,m_iOctave;
};

/// A twelve note series, i.e twelfe times a #Note.
/*! Interface to an array of twelfe \e different notes.

    \sa #CompactSeries
*/
class Series{
 public:
  /// error code for tonal pharses
  /*! \sa #TE_DEFAULT */
  enum TonalError{TE_OK=0,        ///<no error
                  TE_FOURTH=1,    ///<fourth or fifth
                  TE_CHORD=2,     ///<major or minor chord
                  TE_EQ_INTERV=4, ///<three equal intervals in a row
                  TE_ALL_INTERV=8 ///<not all intervales appear in series
                 };
  /*! \sa TonalError */
  enum {TE_DEFAULT=7 ///<same as #TE_FOURTH|#TE_CHORD|#TE_EQ_INTERV
       };
  typedef std::pair<unsigned,TonalError> TonalErrorPos;
  /// plain c'tor 
  /*! \note This c'tor does not initialize, and thus might hold an invalid
            series.
  */
  Series(){}
  /// Construct series form a string
  /*! \param _s input string
      
      Each note is represented by an uppercase char, where \c 'A' corresponds
      to #Note::TONE_MIN and the \c 'L' corsseponds to #Note::TONE_MAX.
      An optional, preceeding lowercase char indicates the octave value,
      where \c 'a' stands for #Note::OCT_MIN
      \ex Series("iGiAiJiKBiEiCiFiIiHiLiD") \end_ex

      \note No error checking is performed.
  */
  explicit Series(const std::string& _s){m_assign(_s);} //'A'..'L'
  /// Return the series as string of length 24.
  /*! For the format of the string cf. #Series(const std::string&)).
  */
  std::string get_string()const;

  /*! \name STL container interface
  */
  //@{
  const Note& at(unsigned _i)const{return m_np[_i];}
        Note& at(unsigned _i)     {return m_np[_i];}
  Note* begin(){return m_np;}
  Note* end(){return m_np+12;}
  const Note* begin()const{return m_np;}
  const Note* end()const{return m_np+12;}
  //@}
 
  /*! \name support for the 48 forms
  */
  //@{
  /// get transposed of the basic form
  Series GetBasicForm(const Note::NoteDiff& _nd=0)const{
   Series s;for(int i=0;i<12;++i)s.at(i)=at(i)+_nd;return s;}
  /// get transposed of the retrograde (backward) form
  Series GetRetrograde(const Note::NoteDiff& _nd=0)const{
   Series s;for(int i=0;i<12;++i)s.at(i)=at(11-i)+_nd;return s;}
  /// get transposed of the inverted (interval inversion) form
  Series GetInversion(const Note::NoteDiff& =0)const;
  /// get normalized form
  /*! The normalized form is the lexicalographical (ignoring octave
      value) minimal form out of the 48 forms. The octave values are choosen
      such, that the first note is either #Note::OCT_NONE or \c 0.
      \sa #IsNormalized()const
  */
  Series GetNormalized()const;
  //@} 

  /// set octave value for all twelve notes to value \p _o
  void set_oct(Note::Octave _o){for(int i=0;i<12;++i)at(i).set_oct(_o);}
  /// delete octave value for all twelve notes to value \p _o
  void set_no_oct(){for(int i=0;i<12;++i)at(i).set_no_oct();}
  /// delete octave value, if any octave value is missing or invalid
  void limit_oct(){for(int i=0;i<12;++i)if(at(i).no_oct() 
   || at(i).get_oct()<Note::OCT_MIN || at(i).get_oct()>Note::OCT_MAX){
   set_no_oct();break;}
  }

  /*! \name find relatives support
  */
  //@{
  /// equivalent to \c next_permutation(10)
  void next_permutation(){std::next_permutation(begin(),end());}
  /// equivalent to \c prev_permutation(10)
  void prev_permutation(){std::prev_permutation(begin(),end());}
  /*! \brief lexicalographical next permutation (ignoring octave information),
             which changes at least position \p _i. */
  void next_permutation(unsigned _i){
   if(_i>9 || !m_next_permutation_at(_i))next_permutation();}
  /*! \brief lexicalographical previous permutation (ignoring octave 
             information), which changes at least position \p _i. */
  void prev_permutation(unsigned _i){
   if(_i>9 || !m_prev_permutation_at(_i))prev_permutation();}
  /// do a step to find valid series (using #next_permutation(unsigned))
  TonalErrorPos step_find_valid_forw(int _iTE){
   return m_step_find_valid(_iTE,&Series::next_permutation);}
  /// do a step to find valid series (using #prev_permutation(unsigned))
  TonalErrorPos step_find_valid_backw(int _iTE){
   return m_step_find_valid(_iTE,&Series::prev_permutation);}
  //@}

  /// returns position of \e first note, which caused error
  /*! \sa FindError(int)const */
  static unsigned transform_error_pos(const TonalErrorPos&);

  /*! \name testing support
  */
  //@{
  /// find tonal error
  /*! \par _iError
           bitwise or'ed combination of tonal error codes (cf. #TonalError).
      \return error position or (\c 0,#TE_OK) if no error
 
      Find the first tonal error, where tonal errors with lower code are
      found first. If #TE_ALL_INTERV is enabled, #TE_FOURTH and #TE_EQ_INTERV
      are ignored. The error position is the last note of the tonal pharse,
      e.g. if you test for #TE_EQ_INTERV and you have "c", "d", "e", "f sharp"
      in your series, the error position would point to "f sharp".
    
      \sa transform_error_pos(const TonalErrorPos&)
  */
  TonalErrorPos FindError(int _iError)const;
  /// return the first note, which occurs twice
  /*! \return \c 0 if ok. Otherwise, the position of the first doubled note
                 (ignoring octave information) is returned.
      \note Series with doubled notes cause undefined behaviour with
            other methods of this class, except the STL container interface
            methods.
  */
  unsigned FindDoubled()const; //0:ok, else:error pos
  ///test, whether series is normalized
  /*! This method tests, whether the series is the lexicalographical smallest
      (ignoring octave information) out of its own 48 forms.
      \sa #GetNormalized()const
  */
  bool IsNormalized()const;
  //@}

  /*! \name comparing support
  */
  //@{
  /// lexicographically compare note values (ignoring octave information)
  bool operator <(const Series& _s)const{return
   std::lexicographical_compare(begin(),end(),_s.begin(),_s.end());}
  /// lexicographically compare note values (ignoring octave information)
  bool operator ==(const Series& _s)const{return
   std::equal(begin(),end(),_s.begin());}
  ///compare both, note and octave value
  bool full_eq(const Series& _s)const{
   for(int i=0;i<12;++i)if(!at(i).full_eq(_s.at(i)))return false;
   return true;
  }
  //@}

 private:
  void m_assign(const std::string&);
  bool m_next_permutation_at(int);
  bool m_prev_permutation_at(int);
  TonalErrorPos m_step_find_valid(int _iTE,
   void(Series::*const perm)(unsigned)){
   TonalErrorPos p=FindError(_iTE);
   if(p.first)(this->*perm)(p.first);return p;
  }
  
  Note m_np[12];
};

#endif
