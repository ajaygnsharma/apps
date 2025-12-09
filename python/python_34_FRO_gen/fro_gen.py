import csv
import configparser
import os
import shutil
import subprocess
import re
import time
import time
from zipfile import ZipFile, ZIP_DEFLATED, ZipInfo

from pathlib import Path

from io import StringIO

def read_config_with_default(filename: str) -> configparser.ConfigParser:
    """Read INI and auto-add [DEFAULT] if missing for simple key=value files."""
    with open(filename, "r") as f:
        content = f.read().strip()
    if not content.startswith("["):
        content = "[DEFAULT]\n" + content
    cfg = configparser.ConfigParser()
    cfg.optionxform = str  # preserve key case
    cfg.read_file(StringIO(content))
    return cfg

def truthy(val: str) -> bool:
    return str(val).strip().strip('"').lower() in {"yes", "true", "1", "y", "on"}

def getq(d: configparser.SectionProxy, key: str, default: str = "") -> str:
    """Get value and strip optional quotes."""
    v = d.get(key, default)
    return v.strip().strip('"')

def normalize_where_used(raw: str) -> list[str]:
    """
    Accepts WHERE_USED as either:
      - actual multiline (indented) entries, or
      - a single line containing '\n' escape sequences.
    Returns a list of cleaned entry strings (still like 'PN, desc, ...').
    """
    if raw is None:
        return []
    # Convert literal "\n" sequences to real newlines so both styles work.
    text = raw.replace("\\n", "\n")
    lines = [ln.strip() for ln in text.splitlines()]
    # Drop empty and surrounding quotes if present.
    cleaned = []
    for ln in lines:
        if not ln:
            continue
        # Many INIs will have each item quoted like: "317-..., Kit, ..."
        if ln.startswith('"') and ln.endswith('"') and len(ln) >= 2:
            ln = ln[1:-1]
        cleaned.append(ln.strip())
    return cleaned

def split_first_comma(entry: str) -> tuple[str, str]:
    """
    Split into (part_number, description) using ONLY the first comma.
    Description may contain commas and should be preserved.
    """
    left, sep, right = entry.partition(",")
    if not sep:  # no comma found
        return entry.strip(), ""
    return left.strip(), right.lstrip()  # lstrip to drop space after first comma


#----------------------------------------------------------------------
# Main logic
#----------------------------------------------------------------------
config = read_config_with_default("cfg.ini")

d = config["DEFAULT"]
model = getq(d, "MODEL")
rev = getq(d, "REV")
rev_readme = getq(d, "REV_README")
family = getq(d, "FAMILY")
serial = getq(d, "SERIAL_NUM")
new_fw = truthy(d.get("NEW_FW", ""))

rows = [
    [f"FWF-{model}-0BIN", f"Firmware, {family} {serial}, BIN", rev],
    [f"FRN-{model}-0BIN", f"Frmwr Rls Notes, {family} {serial}", rev],
    [f"ZIP-{model}-0001", f"ZIP file, {family} {serial}, BIN", rev],
    [f"ZIP-{model}-RM01", f"ZIP file, {family} {serial}, Readme", rev_readme],
]

# If NEW_FW is truthy, parse COMMON_KIT / WHERE_USED and append rows
if new_fw and config.has_section("COMMON_KIT"):
    raw_where_used = config["COMMON_KIT"].get("WHERE_USED", "")
    entries = normalize_where_used(raw_where_used)
    for item in entries:
        pn, desc = split_first_comma(item)
        if pn:  # only add if we have a part number
            rows.append([pn, desc, ""])

#---------------------------------------------------------------------- 
# Write CSV with header; csv module will quote fields with commas 
# automatically
#---------------------------------------------------------------------- 
with open("output.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["File Name", "Description", "Revision"])
    writer.writerows(rows)

print("✅ output.csv generated successfully.")


#----------------------------------------------------------------------
# Put the output of "log.csv" in the FRO log.xlsx file
#----------------------------------------------------------------------
with open("log.csv", "w", newline="") as logf:
    logwriter = csv.writer(logf)
    for r in rows:
        logwriter.writerow([r[0]])

print("✅ output.csv and log.csv generated successfully.")


#----------------------------------------------------------------------
# Create an appropriate FW file as per FRO requirements
#----------------------------------------------------------------------
# --- Copy .bin file based on log.csv ---
assets_dir = "assets"
# Read the first line of log.csv
with open("log.csv", "r") as f:
    first_name = f.readline().strip()

if not first_name:
    print("⚠️ No entries in log.csv; skipping .bin copy.")
else:
    # Find the first .bin file in assets/
    bin_files = [f for f in os.listdir(assets_dir) if f.lower().endswith(".bin")]
    if not bin_files:
        print(f"⚠️ No .bin file found in {assets_dir}/")
    else:
        src = os.path.join(assets_dir, bin_files[0])
        dst = os.path.join(assets_dir, f"{first_name}.bin")
        shutil.copy2(src, dst)
        print(f"✅ Copied {src} → {dst}")


# --- Variables already known from earlier steps ---
assets_dir = "assets"          # where the original .bin file resides
rev = rev.strip('"')           # e.g. "0109"
first_name = first_name.strip()  # e.g. "FWF-23820-0BIN"

# 1️⃣ Rename the file inside assets folder → add REV suffix
old_path = os.path.join(assets_dir, f"{first_name}.bin")
new_filename = f"{first_name}-{rev}.bin"
new_path = os.path.join(assets_dir, new_filename)

if not os.path.exists(old_path):
    raise FileNotFoundError(f"❌ File not found: {old_path}")

shutil.move(old_path, new_path)
print(f"✅ Renamed {old_path} → {new_path}")

# 2️⃣ Parse name for folder structure
parts = first_name.split("-")
if len(parts) < 3:
    raise ValueError(f"Unexpected file name: {first_name}")

_, model, suffix = parts

# 3️⃣ Create full destination folder structure
base_dir = "/mnt/server1/E drive files from Old server/drafting/FWF"
dest_dir = os.path.join(base_dir, model, suffix, rev)
os.makedirs(dest_dir, exist_ok=True)

# 4️⃣ Copy the renamed .bin into the destination folder
dest_file = os.path.join(dest_dir, new_filename)
shutil.copy2(new_path, dest_file)

print(f"✅ Copied {new_path} → {dest_file}")



# assume we already have: model = "23820"
def update_release_notes_conf(model: str, rev: str, family: str) -> Path:
    """
    Locate <MODEL>-Release_Notes/docs/conf.py (or -Release_notes),
    update release, project, and DocDescription, and return docs dir.
    """
    # Be robust to underscore vs lowercase 'notes'
    candidates = [
        Path(f"{model}-Release_Notes") / "docs" / "conf.py",
        Path(f"{model}-Release_notes") / "docs" / "conf.py",
    ]
    conf_path = next((p for p in candidates if p.exists()), None)
    if conf_path is None:
        raise FileNotFoundError(
            f"Could not find conf.py in any of: {', '.join(str(p) for p in candidates)}"
        )

    text = conf_path.read_text(encoding="utf-8")

    # 1) release = "<REV>"
    text, n1 = re.subn(
        r'(^\s*release\s*=\s*)"(.*?)"(\s*)$',
        rf'\1"{rev}"\3',
        text,
        flags=re.MULTILINE,
    )

    # 2) project = "Release Notes for <FAMILY>"
    text, n2 = re.subn(
        r'(^\s*project\s*=\s*)"(.*?)"(\s*)$',
        rf'\1"Release Notes for {family}"\3',
        text,
        flags=re.MULTILINE,
    )

    # 3) \newcommand{\DocDescription}{...}  (inside the raw LaTeX preamble)
    #    Replace ANY existing content with "Release Notes for <FAMILY>"
    text, n3 = re.subn(
        r'(\\newcommand\{\\DocDescription\}\{)(.*?)(\})',
        rf'\1Release Notes for {family}\3',
        text,
        flags=re.DOTALL,
    )

    # Backup + write
    backup = conf_path.with_suffix(".py.bak")
    if not backup.exists():
        shutil.copy2(conf_path, backup)
    conf_path.write_text(text, encoding="utf-8")

    print(
        f"✅ Updated conf.py at {conf_path} "
        f"(release:{n1>0}, project:{n2>0}, DocDescription:{n3>0})"
    )
    return conf_path.parent  # docs dir

def build_release_notes(docs_dir: Path):
    """Build LaTeX PDF with helpful diagnostics."""
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
            cwd=docs_dir.parent,  # …/23820-Release_Notes
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
        print("✅ Release documentation built successfully.")
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



def pick_doc_number_from_log(log_path: str, model: str) -> str:
    p = Path(log_path)
    if not p.exists():
        return f"FRN-{model}-0BIN"
    with p.open("r", encoding="utf-8") as f:
        names = [ln.strip() for ln in f if ln.strip()]
    if not names:
        return f"FRN-{model}-0BIN"
    # Prefer an FRN-* entry; otherwise use the first entry
    for name in names:
        if name.startswith("FRN-"):
            return name
    return names[0]

doc_number = pick_doc_number_from_log("log.csv", model)

# ---- call these after you already have model/rev/family from cfg.ini ----
# Example:
# model = "23820"
# rev   = "0109"
# family = "RXR/RXI"

docs_dir = update_release_notes_conf(model=model, rev=rev, family=family)
build_release_notes(docs_dir)



# ----------------------------------------------------------------------
# Copy the built PDF to FRN/<MODEL>/0001/<REV>/ as FRN-<MODEL>-0001.pdf
# ----------------------------------------------------------------------
latex_dir = docs_dir / "_build" / "latex"
src_pdf = latex_dir / "ibuc_manuals.pdf"

if not src_pdf.exists():
    raise FileNotFoundError(f"Expected PDF not found: {src_pdf}")

frn_base = "/mnt/server1/E drive files from Old server/drafting/FRN"
dest_dir = os.path.join(frn_base, model, "0001", rev)  # .../FRN/23820/0001/0109/
os.makedirs(dest_dir, exist_ok=True)

dest_pdf = os.path.join(dest_dir, f"FRN-{model}-0001-{rev}.pdf")
shutil.copy2(src_pdf, dest_pdf)

print(f"✅ Copied {src_pdf} → {dest_pdf}")




def update_zip_readme_conf(model: str, rev_readme: str, family: str) -> Path:
    """
    Locate <MODEL>-ZIP_Readme/docs/conf.py (or -ZIP_readme),
    set release, project, DocDescription, and return docs dir.
    """
    candidates = [
        Path(f"{model}-ZIP_Readme") / "docs" / "conf.py",
        Path(f"{model}-ZIP_readme") / "docs" / "conf.py",
    ]
    conf_path = next((p for p in candidates if p.exists()), None)
    if conf_path is None:
        raise FileNotFoundError(
            f"Could not find ZIP Readme conf.py in: {', '.join(str(p) for p in candidates)}"
        )

    text = conf_path.read_text(encoding="utf-8")

    # release = "<REV_README>"
    text, _ = re.subn(
        r'(^\s*release\s*=\s*)"(.*?)"(\s*)$',
        rf'\1"{rev_readme}"\3',
        text,
        flags=re.MULTILINE,
    )
    # project = "ZIP Readme for <FAMILY>"
    text, _ = re.subn(
        r'(^\s*project\s*=\s*)"(.*?)"(\s*)$',
        rf'\1"ZIP Readme for {family}"\3',
        text,
        flags=re.MULTILINE,
    )
    # \newcommand{\DocDescription}{...}
    text, n3 = re.subn(
        r'(\\newcommand\{\\DocDescription\}\{)(.*?)(\})',
        rf'\1ZIP Readme for {family}\3',
        text,
        flags=re.DOTALL,
    )
    # If you want a DocNumber on the PDF header too:
    text, n4 = re.subn(
        r'(\\newcommand\{\\DocNumber\}\{)(.*?)(\})',
        rf'\1ZIP-{model}-RM01\3',
        text,
        flags=re.DOTALL,
    )
    if n4 == 0:
        text, _ = re.subn(
            r'(\\newcommand\{\\Author\}\{.*?\}\s*)',
            lambda m: m.group(1) + f'\n\\newcommand{{\\DocNumber}}{{ZIP-{model}-RM01}}\n',
            text,
            flags=re.DOTALL,
        )

    backup = conf_path.with_suffix(".py.bak")
    if not backup.exists():
        shutil.copy2(conf_path, backup)
    conf_path.write_text(text, encoding="utf-8")

    print(f"✅ Updated ZIP Readme conf.py at {conf_path}")
    return conf_path.parent  # docs dir


# Build the ZIP Readme PDF
zip_docs_dir = update_zip_readme_conf(model=model, rev_readme=rev_readme, family=family)
build_release_notes(zip_docs_dir)

# Copy ZIP Readme PDF → /drafting/ZIP/<MODEL>/RM01/<REV_README>/ZIP-<MODEL>-RM01-<REV_README>.pdf
zip_latex_dir = zip_docs_dir / "_build" / "latex"
src_zip_pdf = zip_latex_dir / "ibuc_manuals.pdf"   # Sphinx default name

if not src_zip_pdf.exists():
    raise FileNotFoundError(f"Expected ZIP Readme PDF not found: {src_zip_pdf}")

zip_base = "/mnt/server1/E drive files from Old server/drafting/ZIP"
zip_rm01_dest = os.path.join(zip_base, model, "RM01")
os.makedirs(zip_rm01_dest, exist_ok=True)

# ----------------------------------------------------------------------
# Create ZIP file: ZIP-<MODEL>-0001.zip from assets/zip_content/*
# ----------------------------------------------------------------------
# Create firmware ZIP: assets/ZIP-<MODEL>-0001.zip from assets/zip_content/*
zip_src_dir = Path("assets") / "zip_content"
zip_name = f"ZIP-{model}-0001-{rev}.zip"
zip_path = Path("assets") / zip_name

if zip_src_dir.exists():
    epoch_1980 = time.mktime((1980, 1, 1, 0, 0, 0, 0, 0, -1))
    safe_dt = (1980, 1, 1, 0, 0, 0)

    with ZipFile(zip_path, "w", compression=ZIP_DEFLATED) as zf:
        for root, _, files in os.walk(zip_src_dir):
            for name in files:
                fpath = os.path.join(root, name)
                arcname = os.path.relpath(fpath, zip_src_dir)

                st = os.stat(fpath)
                mtime = max(st.st_mtime, epoch_1980)
                dt = tuple(time.localtime(mtime))[:6]
                if dt[0] < 1980:
                    dt = safe_dt

                zi = ZipInfo(arcname)
                zi.date_time = dt
                zi.compress_type = ZIP_DEFLATED

                with open(fpath, "rb") as fp:
                    zf.writestr(zi, fp.read())

    print(f"✅ Created {zip_path}")

    # Copy firmware ZIP → /drafting/ZIP/<MODEL>/0001/<REV>/
    zip_0001_dest = os.path.join(zip_base, model, "0001", rev)
    os.makedirs(zip_0001_dest, exist_ok=True)
    shutil.copy2(zip_path, os.path.join(zip_0001_dest, zip_name))
    print(f"✅ Copied {zip_name} → {zip_0001_dest}")
else:
    print(f"⚠️ Source folder not found: {zip_src_dir}")


zip_readme_pdf_name = f"ZIP-{model}-RM01.pdf"
shutil.copy2(src_zip_pdf, os.path.join(zip_rm01_dest, zip_readme_pdf_name))

print(f"✅ Copied {src_zip_pdf} → {os.path.join(zip_rm01_dest, zip_readme_pdf_name)}")
