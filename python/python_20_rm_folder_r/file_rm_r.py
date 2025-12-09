import os
import shutil

def remove_directory(target_path, dir_to_remove):
    """
    Traverse the directory tree starting at target_path to find and remove dir_to_remove.

    Parameters:
    - target_path (str): Path of the directory to start the search.
    - dir_to_remove (str): Name of the directory to find and remove.
    """
    for root, dirs, files in os.walk(target_path, topdown=False):
        # Check if the directory to remove is in the current list of directories
        if dir_to_remove in dirs:
            full_path = os.path.join(root, dir_to_remove)
            try:
                shutil.rmtree(full_path)
                print(f"Removed directory: {full_path}")
            except Exception as e:
                print(f"Error removing {full_path}: {e}")

# Example usage
#target_directory = "/home/asharma/PycharmProjects"
target_directory = "/home/asharma/dev/github/ajaygnsharma/tkinter"
directory_to_remove = "venv"

remove_directory(target_directory, directory_to_remove)