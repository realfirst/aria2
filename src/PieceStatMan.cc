/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2009 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "PieceStatMan.h"

#include <algorithm>

#include "SimpleRandomizer.h"

namespace aria2 {

PieceStat::PieceStat(size_t index):_order(0), _index(index), _count(0) {}

bool PieceStat::operator<(const PieceStat& pieceStat) const
{
  if(_count == pieceStat._count) {
    return _order < pieceStat._order;
  } else {
    return _count < pieceStat._count;
  }
}

void PieceStat::addCount()
{
  if(_count < SIZE_MAX) {
    ++_count;
  }
}

void PieceStat::subCount()
{
  if(_count > 0) {
    --_count;
  }
}

size_t PieceStat::getIndex() const
{
  return _index;
}

size_t PieceStat::getCount() const
{
  return _count;
}

void PieceStat::setOrder(size_t order)
{
  _order = order;
}

size_t PieceStat::getOrder() const
{
  return _order;
}

class GenPieceStat {
private:
  size_t _index;
public:
  GenPieceStat():_index(0) {}

  SharedHandle<PieceStat> operator()()
  {
    return SharedHandle<PieceStat>(new PieceStat(_index++));
  }
};

PieceStatMan::PieceStatMan(size_t pieceNum, bool randomShuffle):
  _pieceStats(pieceNum),
  _sortedPieceStatIndexes(pieceNum)
{
  std::generate(_pieceStats.begin(), _pieceStats.end(), GenPieceStat());
  std::vector<SharedHandle<PieceStat> > sortedPieceStats(_pieceStats);
  // we need some randomness in ordering.
  if(randomShuffle) {
    std::random_shuffle(sortedPieceStats.begin(), sortedPieceStats.end(),
			*(SimpleRandomizer::getInstance().get()));
  }
  {
    size_t order = 0;
    for(std::vector<SharedHandle<PieceStat> >::iterator i =
	  sortedPieceStats.begin(); i != sortedPieceStats.end(); ++i) {
      _sortedPieceStatIndexes[order] = (*i)->getIndex();
      (*i)->setOrder(order++);
    }
  }  
}

class PieceStatRarer {
private:
  const std::vector<SharedHandle<PieceStat> >& _pieceStats;
public:
  PieceStatRarer(const std::vector<SharedHandle<PieceStat> >& ps):
    _pieceStats(ps) {}

  bool operator()(size_t lhs, size_t rhs) const
  {
    return _pieceStats[lhs] < _pieceStats[rhs];
  }
};

void PieceStatMan::addPieceStats(const unsigned char* bitfield,
					size_t bitfieldLength)
{
  size_t index = 0;
  for(size_t bi = 0; bi < bitfieldLength; ++bi) {
    
    for(size_t i = 0; i < 8; ++i, ++index) {
      unsigned char mask = 128 >> i;
      if(bitfield[bi]&mask) {
	_pieceStats[index]->addCount();
      }
    }

  }
  std::sort(_sortedPieceStatIndexes.begin(), _sortedPieceStatIndexes.end(),
	    PieceStatRarer(_pieceStats));
}

void PieceStatMan::subtractPieceStats(const unsigned char* bitfield,
					     size_t bitfieldLength)
{
  size_t index = 0;
  for(size_t bi = 0; bi < bitfieldLength; ++bi) {
    
    for(size_t i = 0; i < 8; ++i, ++index) {
      unsigned char mask = 128 >> i;
      if(bitfield[bi]&mask) {
	_pieceStats[index]->subCount();
      }
    }

  }
  std::sort(_sortedPieceStatIndexes.begin(), _sortedPieceStatIndexes.end(),
	    PieceStatRarer(_pieceStats));
}

void PieceStatMan::updatePieceStats(const unsigned char* newBitfield,
					   size_t newBitfieldLength,
					   const unsigned char* oldBitfield)
{
  size_t index = 0;
  for(size_t bi = 0; bi < newBitfieldLength; ++bi) {
    
    for(size_t i = 0; i < 8; ++i, ++index) {
      unsigned char mask = 128 >> i;
      if((newBitfield[bi]&mask) && !(oldBitfield[bi]&mask)) {
	_pieceStats[index]->addCount();
      } else if(!(newBitfield[bi]&mask) && (oldBitfield[bi]&mask)) {
	_pieceStats[index]->subCount();
      }
    }

  }
  std::sort(_sortedPieceStatIndexes.begin(), _sortedPieceStatIndexes.end(),
	    PieceStatRarer(_pieceStats));
}

void PieceStatMan::addPieceStats(size_t index)
{
  std::vector<size_t>::iterator cur =
    std::lower_bound(_sortedPieceStatIndexes.begin(),
		     _sortedPieceStatIndexes.end(),
		     index, PieceStatRarer(_pieceStats));

  _pieceStats[index]->addCount();

  std::vector<size_t>::iterator to =
    std::upper_bound(cur+1, _sortedPieceStatIndexes.end(),
		     index, PieceStatRarer(_pieceStats));
  
  std::rotate(cur, cur+1, to);
}

const std::vector<size_t>& PieceStatMan::getRarerPieceIndexes() const
{
  return _sortedPieceStatIndexes;
}

const std::vector<SharedHandle<PieceStat> >&
PieceStatMan::getPieceStats() const
{
  return _pieceStats;
}

} // namespace aria2