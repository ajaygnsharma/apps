# Makefile for building PDF from reStructuredText sources
#
# Uses: sphinx-build + pdflatex (TeX Live)
# Run: make pdf

TARGET = ibuc_manuals

.PHONY: all pdf clean view help

all: pdf

pdf: $(TARGET).pdf

$(TARGET).pdf: index.rst conf.py
	@echo "==> Building with Sphinx..."
	@mkdir -p _build/sphinx
	@sphinx-build -b latex -q . _build/sphinx 2>&1 | grep -v "WARNING\|NOTE\|^\s*$\|Lexing\|resulted\|Retrying" | head -20 || true
	@if [ ! -f _build/sphinx/$(TARGET).tex ]; then \
		echo "ERROR: LaTeX files not generated"; exit 1; \
	fi

	@echo "==> Running pdflatex..."
	@cd _build/sphinx && xelatex -interaction=batchmode $(TARGET).tex > /dev/null 2>&1
	@if [ -f _build/sphinx/$(TARGET).pdf ]; then \
		cp _build/sphinx/$(TARGET).pdf .; \
		echo "==> PDF created: $(TARGET).pdf ($(shell pdfinfo $(TARGET).pdf 2>/dev/null | grep Pages: | awk '{print $$2}') pages)"; \
	else \
		echo "ERROR: PDF not generated"; exit 1; \
	fi

clean:
	@rm -rf _build *.pdf *.log *.aux *.out *.toc *.idx *.ind *.ilg *.fls *.fdb_latexmk *.synctex.gz 2>/dev/null; \
	echo "Cleaned."

view: $(TARGET).pdf
	@xdg-open $(TARGET).pdf 2>/dev/null || echo "Built $(TARGET).pdf"

help:
	@echo "How to Upgrade Firmware for RXI-CH PDF Builder"
	@echo ""
	@echo "Targets:"
	@echo "  all       Build the PDF (default)"
	@echo "  pdf       Build the PDF"
	@echo "  clean     Remove build artifacts"
	@echo "  view      Build and preview PDF"
	@echo "  help      Show this help"