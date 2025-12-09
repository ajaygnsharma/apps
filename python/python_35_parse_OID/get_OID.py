input_file = "oid.c"       # change as needed
output_file = "only_oids.txt"

with open(input_file, "r") as infile, open(output_file, "w") as outfile:
    for line in infile:
        line = line.strip()
        if line.startswith("// OID .1."):
            outfile.write(line + "\n")
