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

#ifndef UCS2_HXX
#define UCS2_HXX


#include "w_char.hxx"
#include <cstdint>
#include <vector>
#include <string>
#include "csutil.hxx"
#include "ucs_info.hxx"
#include "ucs_digits_data.hxx"
#include "ucs_punct_data.hxx"
#include "langnum.hxx"



namespace ucs {
    const unsigned char w_char_error_high = 0xff;
    const unsigned char w_char_error_low = 0xfd;
    
    inline w_char w_char_error(void){
        w_char u16;
        u16.h = w_char_error_high;
        u16.l = w_char_error_low;
        return u16;
    }

    inline bool is_hi_surrogate_w_char(w_char wchr){
        return ((wchr.h >= 0xd8) && (wchr.h < 0xdb));
    } 

    inline bool is_lo_surrogate_w_char(w_char wchr){
        return ((wchr.h >= 0xdb) && (wchr.h <= 0xdf));
    }

    inline char* nextchar(char* p) {
        if (!p || !*p) return p;
        
        unsigned char c = static_cast<unsigned char>(*p);
        
        if (c < 0x80)           return p + 1;
        if ((c & 0xE0) == 0xC0) return p + 2;
        if ((c & 0xF0) == 0xE0) return p + 3;
        if ((c & 0xF8) == 0xF0) return p + 4;
        
        return p + 1;
    }

    // Macros for future use
    #define UCS_IS_SURROGATE(c)  (((c)>= 0xd800) && ((c) <= 0xdfff)) 
    #define UCS_IS_SINGLE(c)     ((c) < 0xd800)
    #define UCS_IS_LEAD(c)       (((c) >= 0xd800) && ((c) <= 0xdbff))
    #define UCS_IS_TRAIL(c)      (((c) >= 0xdc00) && ((c) <= 0xdfff))
    #define UCS_GET_SUPPLEMENTARY(l, t) (char32_t)((((l) - 0xd800) << 10) | (((t) - 0xdc00) + 0x10000))
    #define UCS_LEAD(c)  ((uint16_t)((((c) - 0x10000) >> 10) + 0xD800))
    #define UCS_TRAIL(c) ((uint16_t)((((c) - 0x10000) & 0x3FF) + 0xDC00))
    #define UCS_IS_UNICODE_CHAR(c) (((c) >= 0 ) && ((c) <= 0x10ffff))
    #define UCS_FROM_LEAD(l) (char32_t)((((l) - 0xd800) << 10) + 0x10000)
    #define UCS_ADD_TRAIL(cp,t) (char32_t)(((cp)) | ((t) - 0xdc00))    
    
    inline w_char uchar_to_w_char(char16_t uni16) {
        w_char u16;
        u16.h = static_cast<unsigned char>((uni16 & 0xff00) >> 8);
        u16.l = static_cast<unsigned char>(uni16 & 0x00ff);
        return u16;
    }

    

/**
 * Reads a UTF-8 character and returns its char32_t codepoint.
 * Advances the source pointer (src) to the beginning of the next character.
 * Returns the replacement character (0xFFFD) if an invalid UTF-8 sequence is encountered.
 */
    inline char32_t utf8_to_utf32_step(const char*& src) {
        if (!src || *src == '\0') return 0;

        uint8_t b1 = static_cast<uint8_t>(*src++);
        
        // 1-byte ASCII sequence (0xxxxxxx)
        if (b1 < 0x80) {
            return b1;
        }
        
        // 2-byte UTF-8 sequence (110xxxxx 10xxxxxx)
        if ((b1 & 0xE0) == 0xC0) {
            if ((static_cast<uint8_t>(*src) & 0xC0) != 0x80) return 0xFFFD; // Invalid continuation byte
            uint8_t b2 = static_cast<uint8_t>(*src++);
            
            char32_t cp = ((b1 & 0x1F) << 6) | (b2 & 0x3F);
            return (cp < 0x80) ? 0xFFFD : cp; // Filter overlong encoding
        }
        
        // 3-byte UTF-8 sequence (1110xxxx 10xxxxxx 10xxxxxx)
        if ((b1 & 0xF0) == 0xE0) {
            if ((static_cast<uint8_t>(src[0]) & 0xC0) != 0x80 || 
                (static_cast<uint8_t>(src[1]) & 0xC0) != 0x80) {
                return 0xFFFD; // Invalid continuation bytes
            }
            uint8_t b2 = static_cast<uint8_t>(*src++);
            uint8_t b3 = static_cast<uint8_t>(*src++);
            
            char32_t cp = ((b1 & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
            
            // Filter overlong encoding (< 0x800) and UTF-16 surrogates (0xD800 - 0xDFFF)
            if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) return 0xFFFD;
            return cp;
        }
        
        // 4-byte UTF-8 sequence - SMP area (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
        if ((b1 & 0xF8) == 0xF0) {
            if ((static_cast<uint8_t>(src[0]) & 0xC0) != 0x80 || 
                (static_cast<uint8_t>(src[1]) & 0xC0) != 0x80 || 
                (static_cast<uint8_t>(src[2]) & 0xC0) != 0x80) {
                return 0xFFFD; // Invalid continuation bytes
            }
            uint8_t b2 = static_cast<uint8_t>(*src++);
            uint8_t b3 = static_cast<uint8_t>(*src++);
            uint8_t b4 = static_cast<uint8_t>(*src++);
            
            char32_t cp = ((b1 & 0x07) << 18) | ((b2 & 0x3F) << 12) | ((b3 & 0x3F) << 6) | (b4 & 0x3F);
            
            // Filter overlong encoding (< 0x10000) and values above Unicode limit (0x10FFFF)
            if (cp < 0x10000 || cp > 0x10FFFF) return 0xFFFD;
            return cp;
        }

        // Invalid lead byte (e.g., standalone continuation byte 0x80-0xBF or out-of-range 0xF5-0xFF)
        return 0xFFFD;
    }
    inline char32_t cast_from_wchrs(w_char lead, w_char trail){
        char16_t h = (char16_t)lead;
        char16_t l = (char16_t)trail;
        return UCS_GET_SUPPLEMENTARY(h,l);
    }

    inline char32_t uc_toupper(char32_t cp) {
        // BMP
        if (cp < 0x10000) {
            return static_cast<char32_t>(unicodetoupper(static_cast<unsigned short>(cp), LANG_xx));
        }
        // SMP area upper bound check
        if (cp > 0x1ffff) {
            return cp;
        }
        //SMP
        size_t index = cp & 0xffff;
        return 0x10000 | ucs_to_upper[index];
    }

    inline char32_t uc_tolower(char32_t cp) {
        // BMP    
        if (cp < 0x10000) {
            return static_cast<char32_t>(unicodetolower(static_cast<unsigned short>(cp), LANG_xx));
        }
        // SMP area upper bound check
        if (cp > 0x1ffff) {
            return cp;
        }
        // SMP
        size_t index = cp & 0xffff;
        return 0x10000 + ucs_to_lower[index];
    }

    std::vector<std::string> init_utf8_vector(const char32_t* data, size_t count);
    std::string codepoint_to_utf8(char32_t cp);
    extern std::vector<std::string> utf8punctuation_marks;
    extern std::vector<std::string> utf8numbers;
    extern std::vector<uint16_t> utfbmp_reserved_codes;
    extern std::vector<uint16_t> utfsmp_reserved_codes;
    bool is_reserved_code(char32_t cp);
    bool is_u16_simple_only(const std::vector<w_char>& src);
    std::vector<char32_t>& u16_u32(std::vector<char32_t>& dest, const std::vector<w_char>& src);
    std::vector<w_char>& u32_u16(std::vector<w_char>& dest, const std::vector<char32_t>& src);
    std::string& u32_u8(std::string& dest, const std::vector<char32_t>& src);
    int u8_u32(std::vector<char32_t>& dest, const std::string& src);
    std::vector<w_char>& ushort_w_char(std::vector<w_char>& dest, const std::vector<unsigned short>& src);
    std::vector<unsigned short>& w_char_ushort(std::vector<unsigned short>& dest, const std::vector<w_char>& src);
    int u8_u16(std::vector<unsigned short>& dest,const std::string& src,bool only_convert_first_letter);
    std::string& u16_u8(std::string& dest, const std::vector<unsigned short>& src);
    uint32_t fnv1a_32_utf32(const std::vector<uint32_t>& data);
    uint32_t fnv1a_32_utf8(const std::string& str);
} // namespace ucs

#endif // UCSSPELL_UCS2_HXX