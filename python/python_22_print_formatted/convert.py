# Read input file (or from a multiline string)
with open("out.log", "r") as infile:
    lines = infile.readlines()

# Remove blank or whitespace-only lines
cleaned_lines = [line for line in lines if line.strip() != ""]

# Optionally write to a new file
with open("cleaned_output.txt", "w") as outfile:
    outfile.writelines(cleaned_lines)

