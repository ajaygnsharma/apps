import os
import subprocess

def rsync_directory(source_directory, destination_directory):
    for root, dirs, files in os.walk(source_directory):
        for directory in dirs:
            source_path = os.path.join(root, directory)
            rsync_command = ('rsync', '-avz', '"-e ssh"', source_path,
                             destination_directory)
            #print(rsync_command)
            subprocess.run(rsync_command)


source_directory: str = "/mnt/128GB_sd_card/Phone_backup/pixel_2xl/DCIM/Camera/"
destination_directory = "user1@debian.local:/mnt/8TB/sorted/images/DSLR/"
#source_directory = "/home/asharma/tmp/"
#destination_directory = "asharma@10.10.8.204:/home/asharma/tmp/"

rsync_directory(source_directory, destination_directory)
