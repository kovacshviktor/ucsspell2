ACLOCAL_AMFLAGS = -I m4

SUBDIRS= po src man tests

pkgconfdir = $(libdir)/pkgconfig
pkgconf_DATA = ucsspell.pc

EXTRA_DIST = license.myspell license.hunspell \
        ChangeLog.O COPYING.LESSER COPYING.MPL ucsspell.pc.in \
    	Doxyfile.in

EXTRA_DIST += scripts/parse_digits.py \
              scripts/parse_ucd.py \
              scripts/parse_smp_casemap.py \
              scripts/parse_reserved_codepoints.py \
              scripts/UnicodeData.txt \
			  scripts/iso15924.txt

# --- Docs target ---
DOCS_DIR = docs_platform
DOXYFILE_IN = Doxyfile.in
DOXYFILE = Doxyfile
PDF_OUTPUT = $(DOCS_DIR)/refman.pdf

docs:
	@echo "Generating Doxygen documentation..."
	@mkdir -p $(DOCS_DIR)
	@cp $(DOXYFILE_IN) $(DOXYFILE)
	@doxygen $(DOXYFILE)
	@echo "Building PDF automatically..."
	@cd $(DOCS_DIR)/latex && make >/dev/null 2>&1
	@cp $(DOCS_DIR)/latex/refman.pdf $(PDF_OUTPUT)
	@echo "Docs ready: $(PDF_OUTPUT)"
	@echo "Cleaning intermediate files..."
	@rm -rf $(DOCS_DIR)/html
	@rm -rf $(DOCS_DIR)/flowcharts
	@rm -rf $(DOCS_DIR)/latex
	@rm -f $(DOXYFILE)
	@echo "Temporary files removed, PDF preserved."

clean-docs:
	@echo "Cleaning generated documentation and PDF..."
	@rm -rf $(DOCS_DIR)/html
	@rm -rf $(DOCS_DIR)/flowcharts
	@rm -rf $(DOCS_DIR)/latex
	@rm -f $(PDF_OUTPUT)
	@rm -f $(DOXYFILE)
	@echo "All documentation cleaned."