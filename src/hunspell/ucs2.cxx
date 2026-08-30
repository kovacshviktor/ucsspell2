/* ***** BEGIN LICENSE BLOCK *****
 *
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * ucsspell modifications
 *
 * Copyright (C) 2026 Kovács Viktor (written by Hungarian name order rule)
 *
 * This file has been modified for the ucsspell project.
 * Modifications include Unicode SMP handling, ICU-based case conversion,
 * UTF-16/surrogate-pair support, and related compatibility changes.
 *
 * These modifications are distributed under the same tri-license terms as
 * Hunspell: MPL 1.1/GPL 2.0/LGPL 2.1.
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * THIS SOFTWARE IS PROVIDED BY Kovács Viktor``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * Kovács Viktor OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 ****** END LICENSE BLOCK ***** */

#include <vector>
#include <cstdint>
#include <cstring>
#include <string>
#include <algorithm>
#include "ucs2.hxx"
#include "ucs_info.hxx"
#include "ucs_digits_data.hxx"
#include "ucs_punct_data.hxx"
#include "ucs_reserved_codes.hxx"
#include "w_char.hxx"
#include "csutil.hxx"
#include "atypes.hxx"
#include "langnum.hxx"


using namespace std;


namespace ucs {

std::string codepoint_to_utf8(char32_t cp) {
    std::string out;
    if (cp <= 0x7F) {
        out.push_back(static_cast<char>(cp));
    } else if (cp <= 0x7FF) {
        out.push_back(static_cast<char>(0xC0 | (cp >> 6)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0xFFFF) {
        out.push_back(static_cast<char>(0xE0 | (cp >> 12)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    } else if (cp <= 0x10FFFF) {
        out.push_back(static_cast<char>(0xF0 | (cp >> 18)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
        out.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
    }
    return out;
}    

// Helper to populate vector<string> from char32_t array


    std::vector<std::string> init_utf8_vector(const char32_t* data, size_t count) {
        std::vector<std::string> result;
        result.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            result.push_back(codepoint_to_utf8(data[i]));
        }
        return result;
    }

    // Global/extern vectors inicialization from the ggenerated thumbs
    std::vector<std::string> utf8punctuation_marks = init_utf8_vector(
        UNICODE_PUNCTUATIONS, 
        NUM_UNICODE_PUNCT
    );

    std::vector<std::string> utf8numbers = init_utf8_vector(
        UNICODE_DIGITS, 
        NUM_UNICODE_DIGITS
    );

    std::vector<uint16_t> utfbmp_reserved_codes = std::vector<uint16_t>(
        BMP_RESERVED_CODEPOINTS, BMP_RESERVED_CODEPOINTS + NUM_BMP_RESERVED
    );

    std::vector<uint16_t> utfsmp_reserved_codes = std::vector<uint16_t>(
        SMP_RESERVED_LOWER_CODEPOINTS, SMP_RESERVED_LOWER_CODEPOINTS + NUM_SMP_RESERVED
    );

bool is_reserved_code(char32_t cp) {
    // BMP range: U+0000..U+FFFF
    if (cp < 0x10000) {
        uint16_t bmp_char = static_cast<uint16_t>(cp);
        return std::binary_search(utfbmp_reserved_codes.begin(), utfbmp_reserved_codes.end(), bmp_char);
    }
    // SMP range: U+10000..U+1FFFF 
    else if (cp <= 0x1FFFF) {
        uint16_t smp_char = static_cast<uint16_t>(cp & 0xFFFF);
        return std::binary_search(utfsmp_reserved_codes.begin(), utfsmp_reserved_codes.end(), smp_char);
    }
    // Reject unsupported planes and invalid ranges
    return true;
}
bool is_u16_simple_only(const std::vector<w_char>& src){
    for (auto cp: src){
            if(!(UCS_IS_SINGLE(cp))){
                return false;
            }
    }
    return true;
}

std::string& u32_u8(std::string& dest, const std::vector<char32_t>& src){
    dest.clear();
    dest.reserve(src.size());
    auto u32 = src.begin(), u32_max = src.end();
    
    while (u32 < u32_max){
        char32_t cp = *u32;
        if (cp >= 0 && cp <= 0x10FFFF && !(cp >= 0xD800 && cp <= 0xDFFF)) {
            if (cp <= 0x7F) {
                dest.push_back(static_cast<char>(cp));
            } else if (cp <= 0x7FF) {
                dest.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
                dest.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            } else if (cp <= 0xFFFF) {
                dest.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
                dest.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                dest.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            } else {
                dest.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
                dest.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                dest.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                dest.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            }
        } else {
            HUNSPELL_WARNING(stderr, "UTF-32 encoding error in u32_u8 function. Invalid codepoint.");
            dest.clear();            
            return dest;
        }
        ++u32;
    }
    return dest;
}

int u8_u32(std::vector<char32_t>& dest, const std::string& src){
    dest.clear();
    dest.reserve(src.size());
    size_t ix = 0;
    size_t length = src.size();
    
    while (ix < length) {
        unsigned char c = static_cast<unsigned char>(src[ix++]);
        char32_t cp = 0;
        size_t extra_bytes = 0;

        if (c <= 0x7F) {
            cp = c;
        } else if ((c & 0xE0) == 0xC0) {
            cp = c & 0x1F;
            extra_bytes = 1;
        } else if ((c & 0xF0) == 0xE0) {
            cp = c & 0x0F;
            extra_bytes = 2;
        } else if ((c & 0xF8) == 0xF0) {
            cp = c & 0x07;
            extra_bytes = 3;
        } else {
            HUNSPELL_WARNING(stderr, "UTF-8 encoding error in u8_u32 function. Invalid lead byte.");
            dest.clear();
            cp = 0xfffd;
        }

        if (ix + extra_bytes > length) {
            HUNSPELL_WARNING(stderr, "UTF-8 encoding error in u8_u32 function. Truncated byte sequence.");
            dest.clear();
            cp = 0xffd;
        }

        for (size_t i = 0; i < extra_bytes; ++i) {
            unsigned char b = static_cast<unsigned char>(src[ix++]);
            if ((b & 0xC0) != 0x80) {
                HUNSPELL_WARNING(stderr, "UTF-8 encoding error in u8_u32 function. Invalid trail byte.");
                dest.clear();
                cp = 0xffd;
            }
            cp = (cp << 6) | (b & 0x3F);
        }

        if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) {
            HUNSPELL_WARNING(stderr, "UTF-8 encoding error in u8_u32 function. Out of range codepoint.");
            dest.clear();
            cp = 0xffd;
        }

        dest.push_back(cp);
    }
    int size = static_cast<int>(dest.end() - dest.begin());
    dest.resize(size);
    return dest.size();
}

std::vector<char32_t>& u16_u32(std::vector<char32_t>& dest, const std::vector<w_char>& src){
    dest.clear();
    auto u16 = src.begin();
    auto u16_end = src.end();
    char16_t lead;
    char16_t trail;
    char16_t unichar;
    char32_t codepoint;
    
    while (u16 < u16_end) {
        unichar = (char16_t)(*u16);
        if (UCS_IS_SINGLE(unichar)){
            codepoint = (char32_t)unichar;
            ++u16;
        } else if (UCS_IS_LEAD(unichar)){
            lead = unichar;
            if ((u16 + 1) == u16_end){
                HUNSPELL_WARNING(stderr,"UTF-16 encoding error in function u16_u32. Truncated surrogate chars, trail missing.\n");
                dest.clear();
                return dest;
            }   
            ++u16;
            unichar = (char16_t)(*u16);
            if(UCS_IS_TRAIL(unichar)){
                trail = unichar;
                codepoint = UCS_GET_SUPPLEMENTARY(lead, trail);                
            } else {
                HUNSPELL_WARNING(stderr,"UTF-16 encoding error. Missing UTF-16 trail char.\n");
                dest.clear();
                return dest;
            } 
            ++u16;
        } else {
            HUNSPELL_WARNING(stderr,"UTF-16 encoding error. Unexpected UTF-16 trail char.\n");
            dest.clear();
            return dest;                
        }   
        dest.push_back(codepoint);
    }
    return dest;
}

std::vector<w_char>& u32_u16(std::vector<w_char> & dest, const std::vector<char32_t>& src) {
    dest.clear();
    auto u32 = src.begin();
    auto u32_end = src.end();
    char32_t c;
    while (u32 < u32_end) {
        c = (*u32);
        if (c <= 0xFFFF) {
            dest.push_back(uchar_to_w_char(static_cast<char16_t>(c)));
        } else {
            char16_t lead  = UCS_LEAD(c);
            char16_t trail = UCS_TRAIL(c);
            dest.push_back(uchar_to_w_char(lead));
            dest.push_back(uchar_to_w_char(trail));
        }
        ++u32;
    }
    return dest;
}

uint32_t fnv1a_32_utf32(const std::vector<uint32_t>& data) {
    const uint32_t FNV_prime = 0x01000193;     
    uint32_t hash = 0x811C9DC5;                

    for (uint32_t cp : data) {
        hash ^= (cp & 0xFF);
        hash *= FNV_prime;
        
        hash ^= ((cp >> 8) & 0xFF);
        hash *= FNV_prime;
        
        hash ^= ((cp >> 16) & 0xFF); // JAVÍTVA: 12-ről 16-ra az SMP (pl. Rovásírás) bájtok pontos eltolásához
        hash *= FNV_prime;
        
        hash ^= ((cp >> 24) & 0xFF);
        hash *= FNV_prime;
    }
    return hash;
}

uint32_t fnv1a_32_utf8(const std::string& str) {
    const uint32_t FNV_prime = 0x01000193;
    uint32_t hash = 0x811C9DC5;

    for (char c : str) {
        hash ^= static_cast<uint8_t>(c);
        hash *= FNV_prime;
    }
    return hash;
}

} // namespace ucs