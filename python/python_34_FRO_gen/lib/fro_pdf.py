import subprocess
import shutil

from pathlib import Path

#
# For releases notes only
#
def build_latex_file(docs_dir: Path):
    """
    Build LaTeX PDF with helpful diagnostics.
    1. Search for latexmk, xelatex. These build using sphinx
    2. Clean previous LaTeX build directory to avoid stale artifacts.
    3. Build .tex files via Sphinx.
    4. Run make all-pdf (using latexmk) to generate the final PDF.

    In short: docs/conf.py+.rst -> .tex -> .pdf
    """
    latex_dir = docs_dir / "_build" / "latex"

    # Optional: sanity checks for required tools
    missing = [t for t in ("latexmk", "xelatex") if shutil.which(t) is None]
    if missing:
        raise RuntimeError(
            "Missing required tools: " + ", ".join(missing) +
            "\nInstall TeX Live packages that include them (e.g. texlive-full,"
            " or at least: latexmk texlive-xetex texlive-latex-extra texlive-fonts-extra)."
        )

    # Clean previous latex build
    if latex_dir.exists():
        print(f"🧹 Removing old LaTeX build: {latex_dir}")
        shutil.rmtree(latex_dir)

    # Build .tex via Sphinx
    print("📘 Building LaTeX sources with sphinx-build…")
    try:
        subprocess.run(
            ["sphinx-build", "-b", "latex", "docs", "docs/_build/latex"],
            cwd=docs_dir.parent,  
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
    except subprocess.CalledProcessError as e:
        print("❌ sphinx-build failed. Output:\n" + e.stdout)
        raise

    # Make PDF (use all-pdf target)
    print("🧩 Running make all-pdf (silent latexmk)…")
    try:
        subprocess.run(
            ["make", "-C", "docs/_build/latex", "all-pdf", "LATEXMKOPTS=-silent"],
            cwd=docs_dir.parent,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        print("✅ PDF documentation built successfully.")
    except subprocess.CalledProcessError as e:
        print("❌ make failed. Output:\n" + e.stdout)

        # Tail last .log to show the real TeX error
        try:
            logs = sorted(latex_dir.glob("*.log"),
                          key=lambda p: p.stat().st_mtime,
                          reverse=True)
            if logs:
                log_path = logs[0]
                print(f"\n🔎 Tail of {log_path}:")
                with log_path.open("r", encoding="utf-8", errors="ignore") as lf:
                    lines = lf.readlines()[-150:]
                print("".join(lines))
            else:
                print("No .log files found in latex build directory.")
        except Exception as tail_err:
            print(f"(Could not read log tail: {tail_err})")
        raise

