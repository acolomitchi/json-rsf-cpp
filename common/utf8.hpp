/*
 * utf8.hpp
 *
 *  Created on: 17 Aug 2016
 *      Author: acolomitchi
 */

#ifndef UTF8_HPP_
#define UTF8_HPP_

#ifdef __cplusplus

#include <string>

// copied from http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ in Aug 2016
// under the following license

/*
 *
 * License
 * Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

// Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

struct utf8_services {

  static uint32_t
  decode(std::size_t* state, char32_t* codep, uint8_t byte) {
    static const uint8_t utf8d[]={
        // The first part of the table maps bytes to character classes that
        // to reduce the size of the transition table and create bitmasks.
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
         1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
         7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
         8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

        // The second part is a transition table that maps a combination
        // of a state of the automaton and a character class to a state.
         0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
        12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
        12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
        12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
        12,36,12,12,12,12,12,12,12,12,12,12,
      };

    uint32_t type = utf8d[byte];

    *codep = (*state != UTF8_ACCEPT) ?
      (byte & 0x3fu) | (*codep << 6) :
      (0xff >> type) & (byte);

    *state = utf8d[256 + *state + type];
    return *state;
  }
  // end of the sequence copied from http://bjoern.hoehrmann.de/utf-8/decoder/dfa/

  static bool buffUtf8ToUtf32(
    std::size_t& destLen, char32_t* dest,
    const char* src,
    const char** nextPos
  ) {
    bool toRet= static_cast<bool>(src);
    if(toRet) {
      size_t count;
      char32_t codePoint=0;
      std::size_t state=UTF8_ACCEPT;
      for(count=0;toRet && count<destLen && *src;) {
        decode(&state, &codePoint, *src);
        switch(state) {
        case UTF8_ACCEPT:
          if(NULL!=dest) {
            *dest=codePoint;
            dest++;
          } // else just count it
          codePoint=0;
          count++; src++;
          break;
        case UTF8_REJECT:
          toRet=false;
          break;
        default:
          break;
        }
      }
      toRet=(UTF8_ACCEPT==state); // otherwise we stopped too early, hitting either
                                  // the zero terminator or outright a reject
      destLen=count;
    }
    if(NULL!=nextPos) {
      *nextPos=src;
    }
    return toRet;
  }

  static bool wstrUtf8ToUtf32(
    std::u32string& dest,
    const char* begin, const char* end,
    const char** errPos
  ) {
    bool toRet=static_cast<bool>(begin);
    if(toRet) {
      char32_t codePoint=0;
      std::size_t state=UTF8_ACCEPT;
      for(;toRet && *begin && begin!=end; begin++) {
        decode(&state, &codePoint, *begin);
        switch(state) {
        case UTF8_ACCEPT:
          dest.append(1, codePoint);
          codePoint=0;
          break;
        case UTF8_REJECT:
          toRet=false;
          break;
        default:
          break;
        }
      }
      toRet=(UTF8_ACCEPT==state); // otherwise we stopped too early because
                                  // we hit the null-char terminator, or the end or malformed utf8 sequence
    }
    if(NULL!=errPos) {
      *errPos=begin;
    }
    return toRet;
  }
};

#endif /* __cplusplus */

#endif /* UTF8_HPP_ */
