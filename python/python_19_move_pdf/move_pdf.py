import os
import shutil

def move_pdf_files(source_dir, destination_dir):
    """
    Moves PDF files from source directory to destination directory.

    Parameters:
    - source_dir (str): The directory to search for PDF files.
    - destination_dir (str): The directory to move the found PDF files to.
    """

    # Ensure the destination directory exists; if not, create it.
    if not os.path.exists(destination_dir):
        os.makedirs(destination_dir)

    # Iterate over all files in the source directory.
    for root, _, files in os.walk(source_dir):
        for filename in files:
            if filename.lower().endswith(".pdf"):
                # Construct full file path
                source_file = os.path.join(root, filename)
                destination_file = os.path.join(destination_dir, filename)

                # Move the file
                shutil.move(source_file, destination_file)
                print(f"Moved: {filename}")

# Example usage
source_directory = os.getcwd()
destination_directory = "./doc"
move_pdf_files(source_directory, destination_directory)
