import shutil
import sys
import os

#----------------------------------------------------------------------
# Create an appropriate FW file as per FRO requirements
#
# Copy .bin file based on FRO_log.csv 
# 
# Basically we need to create a custom named .bin file in assets/
# So that its copied to an appropriate location during FRO processing
#----------------------------------------------------------------------
def copy_fw_file(FRO_name: str, fwf_local_path: str, rev: str):
    parts = FRO_name.split("-")
    if len(parts) < 3:
        raise ValueError(f"Unexpected file name: {FRO_name}")

    _, model, suffix = parts

    base_dir = "/mnt/server1/E drive files from Old server/drafting/FWF"
    dest_dir = os.path.join(base_dir, model, suffix, rev)
    os.makedirs(dest_dir, exist_ok=True)

    fwf_name = fwf_local_path.split(os.sep)[-1]  # Get the actual filename from the path
    dest_file = os.path.join(dest_dir, fwf_name)
    shutil.copy2(fwf_local_path, dest_file)

    print(f"✅ Copied {fwf_local_path} → {dest_file}")


def extract_major_minor(rev: str) -> tuple[str, str]:

    if len(rev) <= 2:
        sys.exit("Revision string must be at least 3 characters long to extract major and minor parts.")
    
    # str[start:end] slicing is safe even if rev is shorter than expected, 
    # but we check length above for clarity
    rev_major = rev[0:2]
    rev_minor = rev[2:] # Includes last character
    
    return rev_major, rev_minor



def create_linux_img_filename(model: str, rev: str):
    fwf_files = {
        "23719": ["TXI_sdcard"],
        "23820": ["23820"],
    }

    assets_dir = f"{model}/assets"
    img_full_path = ""
    
    # Read the first line of log.csv
    img_name = fwf_files[model][0]  # e.g. "TXI_sdcard" or "23820"
    
    rev_major, rev_minor = extract_major_minor(rev)

    new_img_filename = f"{img_name}_v{rev_major}_{rev_minor}.img"
    
    FRO_img_filename = f"FWF-{model}-0IMG"


    img_files = []
    for f in os.listdir(assets_dir):
        if f.lower().endswith(".img"):
            img_files.append(f)
        

        if not img_files:
            print(f"No .img file found in {assets_dir}/")
        else:
            src = os.path.join(assets_dir, img_files[0]) # Take first img file
            dst = os.path.join(assets_dir, new_img_filename)
            img_full_path = dst
            shutil.copy2(src, dst)
            print(f"Copied {src} → {dst}")

    return img_full_path, FRO_img_filename

#------------------------------------------------------------------------------
# Create the fwf...bin file locally
#------------------------------------------------------------------------------
def create_fw_filename(model: str):
    assets_dir = f"{model}/assets"
    fwf_full_path = ""
    
    # Read the first line of log.csv
    first_name = ""
    with open("FRO_log.csv", "r") as f:
        first_name = f.readline().strip()

    new_fwf_filename = f"{first_name}-{rev}.bin"
    if not first_name:
        print("No entries in FRO_log.csv; skipping .bin copy.")
    else:
        # Find the first .bin file in assets/
        
        bin_files = [f for f in os.listdir(assets_dir) if f.lower().endswith(".bin")]
        if not bin_files:
            print(f"No .bin file found in {assets_dir}/")
        else:
            src = os.path.join(assets_dir, bin_files[0])
            dst = os.path.join(assets_dir, new_fwf_filename)
            fwf_full_path = dst
            shutil.copy2(src, dst)
            print(f"Copied {src} → {dst}")
    
    return first_name, fwf_full_path, new_fwf_filename





#------------------------------------------------------------------------------
# Create the SWU file locally
# return fwf_full_path: So that is final path to be pused out
#        new_swu_filename, fwf_img_name
# @model: like "23719" or "23820"
# @rev: revision string like "0106" or "0054"
#------------------------------------------------------------------------------
def create_linux_swu_filename(model: str, rev: str):
    
    # Only Linux SOM based assets
    assets_dirs = {
        "23139": f"23139_IBB/assets",
        "23719": f"23719_TXI/assets",
        "23818": f"23818_IBD/assets",
        "23821": f"23821_RXI_CH/assets",
    }

    assets_dir = assets_dirs.get(model, "")
    if not assets_dir:
        print(f"No assets directory found for model {model}; skipping .swu creation.")
        return "", ""
    

    swu_full_path = ""
    
    # Read the first line of log.csv
    fwf_files = {
        "23719": ["TXI"],
        "23820": ["23820"],
    }
    swu_name = fwf_files[model][0]  # e.g. "TXI_" or "23820_"

    rev_major, rev_minor = extract_major_minor(rev)

    new_swu_filename = f"{swu_name}_v{rev_major}_{rev_minor}.swu"
    
    FRO_swu_filename = f"FWF-{model}-0SWU"
    

    if not swu_name:
        print("No entries in FRO_log.csv; skipping .swu copy.")
        return "", ""
    
    else:
        swu_files = []
        for f in os.listdir(assets_dir):
            if f.lower().endswith(".swu"):
                swu_files.append(f)

        # Find the first .swu file in assets/
        if not swu_files:
            print(f"No .swu file found in {assets_dir}/")
        else:
            src = os.path.join(assets_dir, swu_files[0]) # Take first .swu file
            dst = os.path.join(assets_dir, new_swu_filename)
            swu_full_path = dst

            shutil.copy2(src, dst)
            print(f"Copied {src} → {dst}")
    

    return swu_full_path, FRO_swu_filename

