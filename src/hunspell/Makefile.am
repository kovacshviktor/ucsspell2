lib_LTLIBRARIES = libucsspell-1.7.la
libucsspell_1_7_includedir = $(includedir)/hunspell
libucsspell_1_7_la_SOURCES = affentry.cxx affentry.hxx affixmgr.cxx affixmgr.hxx \
	atypes.hxx baseaffix.hxx csutil.cxx csutil.hxx ucs2.cxx ucs2.hxx ucs_info.hxx \
	filemgr.cxx filemgr.hxx hashmgr.cxx hashmgr.hxx \
	htypes.hxx hunspell.cxx hunspell.h hunspell.hxx \
	hunzip.cxx hunzip.hxx langnum.hxx \
	phonet.cxx phonet.hxx replist.cxx replist.hxx \
	suggestmgr.cxx suggestmgr.hxx utf_info.hxx w_char.hxx \
	hunspelltrace.cxx hunspelltrace.hxx hunversion.h \
	ucs_digits_data.hxx ucs_punct_data.hxx ucs_reserved_codes.hxx
nodist_libucsspell_1_7_la_SOURCES = hunvisapi.h

libucsspell_1_7_include_HEADERS = hunspell.hxx hunspell.h hunvisapi.h \
	hunversion.h w_char.hxx atypes.hxx

libucsspell_1_7_la_LDFLAGS = -no-undefined -version-info 0:1:0

AM_CXXFLAGS = $(CFLAG_VISIBILITY) -DBUILDING_LIBHUNSPELL

# --- Unicode Table Generation Rules ---

UC_DATA_URL = https://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
UC_SCRIPT_ID_URL = https://www.unicode.org/iso15924/iso15924.txt

# Directory containing scripts and local UnicodeData.txt
SCRIPT_DIR = $(top_srcdir)/scripts
UC_DATA_FILE = $(SCRIPT_DIR)/UnicodeData.txt
UC_SCRIPT_ID_FILE = $(SCRIPT_DIR)/iso15924.txt

PARSE_DIGITS_SCRIPT = $(SCRIPT_DIR)/parse_digits.py
PARSE_PUNCT_SCRIPT = $(SCRIPT_DIR)/parse_ucd.py
PARSE_SMP_SCRIPT = $(SCRIPT_DIR)/parse_smp_casemap.py
PARSE_UCS_RESERVED_SCRIPT = $(SCRIPT_DIR)/parse_reserved_codepoints.py

# Target generated headers in src/hunspell/
GEN_DIGITS_HDR = ucs_digits_data.hxx
GEN_PUNCT_HDR = ucs_punct_data.hxx
GEN_SMP_HDR = ucs_info.hxx
GEN_RESERVED_HDR = ucs_reserved_codes.hxx

GEN_UNICODE_FILES = $(GEN_DIGITS_HDR) $(GEN_PUNCT_HDR) $(GEN_SMP_HDR) $(GEN_RESERVED_HDR)

.PHONY: fetch-unicode-data run-codegen

# Download the latest UnicodeData.txt and iso15924.txt into the scripts directory if enabled
fetch-unicode-data:
if ENABLE_ONLINE_UC_DATA
	@echo "Fetching latest UnicodeData.txt into $(SCRIPT_DIR)..."
	@if command -v curl >/dev/null 2>&1; then \
		curl -sS $(UC_DATA_URL) -o $(UC_DATA_FILE); \
	else \
		wget -q $(UC_DATA_URL) -O $(UC_DATA_FILE); \
	fi
	@echo "Fetching latest iso15924.txt into $(SCRIPT_DIR)..."
	@if command -v curl >/dev/null 2>&1; then \
		curl -sS $(UC_SCRIPT_ID_URL) -o $(UC_SCRIPT_ID_FILE); \
	else \
		wget -q $(UC_SCRIPT_ID_URL) -O $(UC_SCRIPT_ID_FILE); \
	fi
endif

# Execute Python scripts inside SCRIPT_DIR so LOCAL_FILE = "UnicodeData.txt" resolves correctly
$(GEN_UNICODE_FILES): fetch-unicode-data
if ENABLE_PYTHON_CODEGEN
	@echo "Regenerating C++ Unicode tables via Python scripts..."
	(cd $(SCRIPT_DIR) && $(PYTHON) parse_digits.py)
	(cd $(SCRIPT_DIR) && $(PYTHON) parse_ucd.py)
	(cd $(SCRIPT_DIR) && $(PYTHON) parse_smp_casemap.py)
	(cd $(SCRIPT_DIR) && $(PYTHON) parse_reserved_codepoints.py)
else
	@echo "Skipping Python code generation. Using pre-generated Unicode headers."
endif

# Ensure generated headers are built before compilation and included in distribution tarballs
BUILT_SOURCES = $(GEN_UNICODE_FILES)
EXTRA_DIST = $(GEN_UNICODE_FILES)