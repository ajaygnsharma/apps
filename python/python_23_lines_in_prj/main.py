import pathlib
from os import walk

prj = ["/home/asharma/ibuc_linux/adf5356",
       "/home/asharma/ibuc_linux/buildroot-external-terrasat",
       "/home/asharma/ibuc_linux/cli",
       "/home/asharma/ibuc_linux/core",
       "/home/asharma/ibuc_linux/default",
       "/home/asharma/ibuc_linux/integrity",
       "/home/asharma/ibuc_linux/leds",
       "/home/asharma/ibuc_linux/lib",
       "/home/asharma/ibuc_linux/misc",
       "/home/asharma/ibuc_linux/nm",
       "/home/asharma/ibuc_linux/nodejspass",
       "/home/asharma/ibuc_linux/relays",
       "/home/asharma/ibuc_linux/snmp",
       "/home/asharma/ibuc_linux/statlog",
       "/home/asharma/ibuc_linux/uptime",
       "/home/asharma/ibuc_linux/webserver/routes",
       "/home/asharma/ibuc_linux/webserver/views"];

accepted_extensions = [".c", ".cpp", ".h", ".js", ".ejs", ".mk", ".in", ".ac", ".am"];

DEBUG_PRINT_FILES=0;

def countLines(fileName):
    with open(fileName, 'r') as fp:
        for count, line in enumerate(fp):
            pass

    return count+1;

def countLinesInFiles(files):
    totalProjectLines = 0;
    for f in files:
        file_extension = pathlib.Path(f).suffix;
        if(file_extension in accepted_extensions):
            if(DEBUG_PRINT_FILES):
                print(f, sep='\n');
            totalProjectLines += countLines(f);

    return totalProjectLines;


def buildFileList(prjPath):
    files = []
    for (dirpath, dirnames, filenames) in walk(prjPath):
        fullpathList = [];
        for f in filenames:
            fullpath=dirpath + "/" + f;
            fullpathList.append(fullpath);

        files.extend(fullpathList);

    return files;


grandTotal=0

for p in prj:
    fileList   = buildFileList(p);
    totalLines = countLinesInFiles(fileList);
    grandTotal += totalLines;
    print("Project: %s, lines=%d" % (p, totalLines));

print("Grand Total=%d"%(grandTotal));




