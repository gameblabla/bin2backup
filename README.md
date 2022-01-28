# BIN2BACKUP

BIN2BACKUP is a tool for allowing you to convert any liberis-compiled executable through 
the backup memory port used by devices like the FX-BMP.

It is possible to execute homebrew code through either the backup memory by embedding a special header to the executable.
If done sucessfully, it will boot right after the PC-FX logo.

Documentation says that up to 128MB is addressable in memory.
However, it seems that the real limit is lower than that, probably half that amount.
Additionally, you cannot execute code from ROM : you have to copy it to RAM.
While it is still in theory possible to load data from ROM, this does make it a bit more difficult for a 
simple tool like this one. (Short of manually specifying the offset where the data is)

Therefore, i have decided to limit the max size to 2MB, which is the PC-FX's total RAM size.
(But you will want to target slightly less than that)

# How to use

./bin2backup.elf myrom.bin finalrom.sav

# PCFX Backup memory format

0x28 (8 byte)	"PCFXBoot" (Not "PCFXBOOT" as described in Marshall's documentation)

0x30 (1 word)	Backup memory source offset (byte offset)

0x34 (1 word)	RAM destination

0x38 (1 word)	Transfer size [in bytes]

0x3C (1 word)	RAM run length/Starting PC (my tool sets it to the same as RAM destination)

