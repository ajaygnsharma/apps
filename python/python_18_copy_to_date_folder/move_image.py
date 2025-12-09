import os
from datetime import datetime
import shutil
from PIL import Image
import subprocess
import re

def get_image_date(image_path):
    try:
        with Image.open(image_path) as img:
            exif_data = img._getexif()
            if exif_data and 36867 in exif_data:
                date_str = exif_data[36867]
                return datetime.strptime(date_str, '%Y:%m:%d %H:%M:%S').date()
    except Exception as e:
        print(f"Error reading exif data for {image_path}: {e}")
    return None

def get_file_modification_date(file_path):
    try:
        modification_time = os.path.getmtime(file_path)
        return datetime.fromtimestamp(modification_time).date()
    except Exception as e:
        print(f"Error getting modification date for {file_path}: {e}")
        return None

def get_file_creation_date(file_path):
    try:
        creation_time = os.path.getctime(file_path)
        return datetime.fromtimestamp(creation_time).date()
    except Exception as e:
        print(f"Error getting creation date for {file_path}: {e}")
        return None

def parse_file_date(file_path):
    filename = os.path.basename(file_path);
    match = re.search(r'(\d{4})(\d{2})(\d{2})', filename)
    if match:
        year, month, day = match.groups()
        dir_name = f"{year}-{month}-{day}"
        return dir_name;


def move_images_to_date_folders(source_folder):
    for root, dirs, files in os.walk(source_folder):
        for file in files:
            if file.lower().endswith('.jpeg') or file.lower().endswith('.jpg') or file.lower().endswith('.dng'):
                image_path = os.path.join(root, file)
                #date = get_file_modification_date(image_path); 
                #date = get_file_creation_date(image_path); 
                #date = get_image_date(image_path);
                date = parse_file_date(image_path);
                if date:
                    date_folder = os.path.join(source_folder, str(date))
                    os.makedirs(date_folder, exist_ok=True)
                    shutil.move(image_path, os.path.join(date_folder, file))
                    print(f"Moved {file} to {date_folder}")



def rsync_directory(source_directory, destination_directory):
    for root, dirs, files in os.walk(source_directory):
        for directory in dirs:
            source_path = os.path.join(root, directory)
            rsync_command = ('rsync', '-avz', '"-e ssh"', source_path,
                             destination_directory)
            #print(rsync_command)
            subprocess.run(rsync_command)

'''
'''

#source_folder = "/home/ajay/Autosync/"
#source_folder = "/mnt/8TB/unsorted/phone_backups/android_motorola_mini/Camera/"
#source_folder= "/mnt/8TB/unsorted/phone_backups/android_motorola_mini/whatsapp/Media/"
#source_folder= "/mnt/8TB/unsorted/phone_backups/android_motorola_mini/whatsapp/Media/WhatsAppImages/"

source_folder=os.getcwd()
#source_directory: str = "/home/ajay/Autosync/"
#destination_directory = "user1@debian.local:/mnt/8TB/sorted/images/DSLR/"

move_images_to_date_folders(source_folder)
#rsync_directory(source_directory, destination_directory)


#source_directory: str = "/mnt/128GB_sd_card/Phone_backup/pixel_2xl/DCIM/Camera/"
#source_directory = "/home/asharma/tmp/"
#destination_directory = "asharma@10.10.8.204:/home/asharma/tmp/"


#file_path = "path/to/your/image.jpg"
#creation_date = get_file_creation_date(file_path)
#modification_date = get_file_modification_date(file_path)

#print("Modification Date:", modification_date)
#print("Creation Date:", creation_date)

