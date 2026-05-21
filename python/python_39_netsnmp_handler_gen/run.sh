rm -rf ./out

python ./mib_autogen.py tx21MIB oids.csv tables.csv table_columns.csv ./out/
python ./mib_autogen.py tx21MIB scalar_oids_redundancy.csv tables.csv bucSensorTable_columns.csv ./out/


python ./mib_autogen.py tx21MIB scalar_oids_info.csv tables.csv bucTxTable_columns.csv ./out/