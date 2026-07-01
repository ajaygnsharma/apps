import csv
import configparser
import os
import shutil
import sys
from pathlib import Path
from io import StringIO

from lib.fro_pdf import build_latex_file
from lib.release_notes import update_release_notes_conf
from lib.zip_readme import update_zip_readme_conf, copy_zip_readme_pdf
from lib.fwf import copy_fw_file, create_linux_swu_filename, create_linux_img_filename, create_fw_filename



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



def getA(d: configparser.SectionProxy, key: str, default: str = "") -> str:
    """Get value and strip optional quotes."""
    v   = d.get(key, default)
    arr = [ s.strip() for s in v.split(",")]
    return arr



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


#----------------------------------------------------------------------
# Build the ZIP Readme PDF
#----------------------------------------------------------------------
def build_zip_readme_file(model: str, rev_readme: str, family: str):
    zip_docs_dir = update_zip_readme_conf(model=model, rev_readme=rev_readme, family=family)
    build_latex_file(zip_docs_dir)



#----------------------------------------------------------------------
# Build Release Notes PDF
#----------------------------------------------------------------------
def build_release_notes_file(model: str, rev: str, family: str):
    doc_number = pick_doc_number_from_log("FRO_log.csv", model)

    docs_dir = update_release_notes_conf(model=model, rev=rev, family=family)
    build_latex_file(docs_dir)
    return docs_dir



#------------------------------------------------------------------------------
# Create the Output.csv file
#------------------------------------------------------------------------------
def generate_output_csv():
    config = read_config_with_default("cfg.ini")

    d          = config["DEFAULT"]
    model      = getq(d, "MODEL")
    rev        = getq(d, "REV")
    rev_readme = getq(d, "REV_README")
    family     = getA(d, "FAMILY")
    serial     = getA(d, "SERIAL_NUM")
    new_fw     = truthy(d.get("NEW_FW", ""))
    linux      = truthy(d.get("LINUX", ""))

    rows = []
    if linux:
        if len(family) == 1:
            rows = [
                [f"FWF-{model}-0SWU", f"Firmware, {family} {serial}, SWU", rev],
                [f"FWF-{model}-0IMG", f"Firmware, {family} {serial}, IMG", rev],
                [f"FRN-{model}-0BIN", f"Frmwr Rls Notes, {family} {serial}", rev],
                [f"ZIP-{model}-0001", f"ZIP file Frmwr Upgrd Pckg, {family} {serial}, SWU", rev],
                [f"ZIP-{model}-RM01", f"ZIP file, {family} {serial}, Readme", rev_readme],
                [f'729-{model}-0001', f"SD Card, {family} {serial}, Programmed", rev],
            ]
        else:
            print("Check cfg.ini for incorrect Family info")
            sys.exit()
    else:
        if len(family) == 1:
            rows = [
                [f"FWF-{model}-0BIN", f"Firmware, {family} {serial}, BIN", rev],
                [f"FRN-{model}-0BIN", f"Frmwr Rls Notes, {family} {serial}", rev],
                [f"ZIP-{model}-0001", f"ZIP file, {family} {serial}, BIN", rev],
                [f"ZIP-{model}-RM01", f"ZIP file, {family} {serial}, Readme", rev_readme],
            ]
        else:
            # Dont change the format string for Firmmware, ... it needs to match exactly
            # there seems to be some space restrictions on FRO side when parsing these fields
            rows = [
                [f"FWF-{model}-0BIN", f"Firmware,{family[0]} {serial[0]},{family[1]} {serial[1]}, BIN", rev],
                [f"FRN-{model}-0BIN", f"Frmwr Rls Notes,{family[0]} {serial[0]},{family[1]} {serial[1]}",      rev],
                [f"ZIP-{model}-0001", f"ZIP file,{family[0]} {serial[0]},{family[1]} {serial[1]}, BIN", rev],
                [f"ZIP-{model}-RM01", f"ZIP file,{family[0]} {serial[0]},{family[1]} {serial[1]}, Readme", rev_readme],
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
    # automatically. This is the file used to populate actual FRO .
    #---------------------------------------------------------------------- 
    with open("FRO_output.csv", "w", newline="") as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(["File Name", "Description", "Revision"])
        writer.writerows(rows)

    print("✅ FRO_output.csv generated successfully.")

    return rows, model, rev, rev_readme, family, serial, new_fw, linux

#----------------------------------------------------------------------
# Put the output of "log.csv" in the FRO log.xlsx file
#----------------------------------------------------------------------
def generate_log_csv(rows):
    with open("FRO_log.csv", "w", newline="") as logf:
        logwriter = csv.writer(logf)
        for r in rows:
            logwriter.writerow([r[0]])
    print("✅ FRO_log.csv generated successfully.")

#----------------------------------------------------------------------
# Generate output.csv

# Main logic
#----------------------------------------------------------------------
if __name__ == "__main__":
    
    # Generate Output.csv
    rows, model, rev, rev_readme, family, serial, new_fw, linux = generate_output_csv()

    # Generate FRO_log.csv
    generate_log_csv(rows)

    
    if linux:
        swu_local_path, FRO_swu_filename = create_linux_swu_filename(model=model, rev=rev)
        img_local_path, FRO_img_filename = create_linux_img_filename(model=model, rev=rev)

        if not swu_local_path:
            print("No firmware file found.")
            exit(1)
        
        # For Linux copy both SWU and IMG files
        copy_fw_file(FRO_name=FRO_swu_filename, fwf_local_path=swu_local_path,  rev=rev)
        copy_fw_file(FRO_name=FRO_img_filename, fwf_local_path=img_local_path,  rev=rev)

        docs_dir = build_release_notes_file(model=model, rev=rev, family=family)
        
        latex_dir = docs_dir / "_build" / "latex"
        src_pdf = latex_dir / "ibuc_manuals.pdf"

        if not src_pdf.exists():
            raise FileNotFoundError(f"Expected PDF not found: {src_pdf}")

        frn_base = "/mnt/server1/E drive files from Old server/drafting/FRN"
        dest_dir = os.path.join(frn_base, model, "0SWU", rev)  # .../FRN/23820/0001/0109/
        os.makedirs(dest_dir, exist_ok=True)

        dest_pdf = os.path.join(dest_dir, f"FRN-{model}-0001-{rev}.pdf")
        shutil.copy2(src_pdf, dest_pdf)

        print(f"✅ Copied {src_pdf} → {dest_pdf}")

    else:
        first_name, fwf_local_path, fwf_name = create_fw_filename(model=model)
        #copy_fw_file(first_name=first_name, fwf_local_path=fwf_local_path, fwf_name=fwf_name, rev=rev)

    # Work on Zip file
    build_zip_readme_file(model=model, rev_readme=rev_readme, family=family)
    copy_zip_readme_pdf(model=model, rev=rev, rev_zip=rev_readme, zip_docs_dir=Path(f"{model}") / "ZIP_Readme" / "docs")
    

    # ----------------------------------------------------------------------
    # Copy the built PDF to FRN/<MODEL>/0001/<REV>/ as FRN-<MODEL>-0001.pdf
    # ----------------------------------------------------------------------
    


    

