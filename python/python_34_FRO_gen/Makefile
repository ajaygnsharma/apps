# ------- config -------
PY      ?= python3
CFG     ?= cfg.ini
SCRIPT  ?= fro_gen.py
LOGDIR  ?= build_logs
LOGFILE ?= $(LOGDIR)/release.$(shell date +%Y%m%d-%H%M%S).log
VENV    ?= .venv

# Optional: Sphinx & helpers for a tidy local env
REQS    ?= sphinx==7.* myst-parser==2.*  # add others if you need
PYBIN   := $(VENV)/bin/python
PIPBIN  := $(VENV)/bin/pip

# ------- phony targets -------
.PHONY: release setup env check-tools docs zip clean deepclean help

## default entry point
release: check-tools
	@mkdir -p $(LOGDIR)
	@echo "🔧 Running pipeline: $(SCRIPT) (cfg=$(CFG))"
	@$(PY) $(SCRIPT) 2>&1 | tee $(LOGFILE)
	@echo "📝 Log saved to: $(LOGFILE)"

## create a local venv & install python deps (Sphinx)
setup:
	@test -d $(VENV) || python3 -m venv $(VENV)
	@$(PIPBIN) -q install --upgrade pip
	@$(PIPBIN) -q install $(REQS)
	@echo "✅ venv ready → activate with: source $(VENV)/bin/activate"

## print versions for quick sanity checks
env:
	@echo "Python: $$($(PY) -V)"
	@echo "Which python: $$(which $(PY))"
	@echo "latexmk: $$(command -v latexmk || echo 'MISSING')"
	@echo "xelatex: $$(command -v xelatex || echo 'MISSING')"

## ensure latexmk & xelatex are available (fast fail if missing)
check-tools:
	@command -v latexmk >/dev/null || (echo "❌ latexmk not found"; exit 1)
	@command -v xelatex  >/dev/null || (echo "❌ xelatex not found"; exit 1)
	@echo "✅ Tools OK (latexmk & xelatex found)"

## (optional) build only docs via the script sections
docs:
	@$(PY) $(SCRIPT) --docs-only

## (optional) build only the ZIP bundle via the script sections
zip:
	@$(PY) $(SCRIPT) --zip-only

## clean local byproducts (does NOT touch network share)
clean:
	@rm -rf assets/*.bin assets/*.zip
	@find . -path "*/docs/_build" -type d -prune -exec rm -rf {} +
	@echo "🧹 Cleaned local outputs."

## deep clean everything generated locally (careful)
deepclean: clean
	@rm -rf $(LOGDIR) $(VENV)
	@echo "🧨 Deep cleaned logs & venv."

help:
	@echo "Targets:"
	@echo "  make release     - run the full pipeline (CSV, BIN, PDFs, ZIP, copies)"
	@echo "  make setup       - create venv and install Sphinx"
	@echo "  make env         - print versions"
	@echo "  make check-tools - verify latexmk/xelatex present"
	@echo "  make docs        - only docs section (optional hook)"
	@echo "  make zip         - only ZIP bundle (optional hook)"
	@echo "  make clean       - remove local outputs"
	@echo "  make deepclean   - remove logs + venv too"
