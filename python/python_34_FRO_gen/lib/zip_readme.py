from pathlib import Path
import re
import shutil
from zipfile import ZipFile, ZIP_DEFLATED, ZipInfo
import os

#------------------------------------------------------------------------------
# Update conf.py 
#------------------------------------------------------------------------------
def update_zip_readme_conf(model: str, rev_readme: str, family: str) -> Path:
    """
    Locate <MODEL>-ZIP_Readme/docs/conf.py (or -ZIP_readme),
    set release, project, DocDescription, and return docs dir.
    """
    candidates = [
        Path(f"{model}") / "ZIP_Readme" / "docs" / "conf.py",
        Path(f"{model}") / "ZIP_readme" / "docs" / "conf.py",
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


#----------------------------------------------------------------------
# Copy the ZIP Readme PDF to the drafting directory
#----------------------------------------------------------------------
def copy_zip_readme_pdf(model: str, rev: str, rev_zip: str, zip_docs_dir: Path):
    # Copy ZIP Readme PDF → /drafting/ZIP/<MODEL>/RM01/<REV_README>/ZIP-<MODEL>-RM01-<REV_README>.pdf
    zip_latex_dir = zip_docs_dir / "_build" / "latex"
    src_zip_pdf = zip_latex_dir / "ibuc_manuals.pdf"   # Sphinx default name

    if not src_zip_pdf.exists():
        raise FileNotFoundError(f"Expected ZIP Readme PDF not found: {src_zip_pdf}")

    zip_base = "/mnt/server1/E drive files from Old server/drafting/ZIP"
    zip_rm01_dest = os.path.join(zip_base, model, "RM01", f"Rev{rev_zip}")
    os.makedirs(zip_rm01_dest, exist_ok=True)

    # ----------------------------------------------------------------------
    # Create ZIP file: ZIP-<MODEL>-0001.zip from assets/zip_content/*
    # ----------------------------------------------------------------------
    # Create firmware ZIP: assets/ZIP-<MODEL>-0001.zip from assets/zip_content/*
    zip_src_dir = Path(f"{model}") /"assets" / "zip_content"
    zip_name = f"ZIP-{model}-0001-{rev}.zip"
    zip_path = Path(f"{model}") /"assets" / zip_name

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

