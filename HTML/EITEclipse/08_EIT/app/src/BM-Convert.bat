objcopy --input-target=ihex --output-target=binary Objects\EIT.hex Objects\EIT.bin
uTaskerConvert.exe Objects\EIT.bin Upload\EIT_upload.bin -0x22ba -b267a8200926