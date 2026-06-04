--------------------------------------------
NGII Save File Update Utility by ike9000
--------------------------------------------

	Program to update modified Ninja Gaiden II game-save
	files so that they can be accepted by the game.
	For mid- or transient-chapter saves.
	
	Compatible with saves created on Xenia emulator.
	Xbox 360 saves are partially supported.
	Command line utility.


Usage Instructions
--------------------------
	
	Simply, provide path to the save file on the command line.
	Path must be to the dot-dat file itself and not, for example,
	to the directory.
	
	Example:
	
		sg_util_ng_ii.exe "c:\path\to\ng2stryd19.dat"


Basic Hex Editing Information
---------------------------------------

	This program does not make any modifications to the saves besides
	updating a 32-bit value that is a checksum of all the 
	other 32-bit values.


Example save file editing
----------------------------------

	Open the save in hex editor.
	Go to file offset 48 (decimal).
	This is a slot where the code of the first inventory item is stored.
	All item codes are made out of 4 bytes. There can be maximum of 128 items.
	All codes that are set to [00 00 00 00] are free slots, 
	the game will skip over them. This first address, almost certainly, 
	will correspond to Dragon Sword. Change it to [00 07 01 00] to 
	have Dragon Sword level 4 or change it to [00 01 01 01] to have 
	Dragon Sword level 2. (Codes for DS level 1 and 3 are [00010100] 
	and [00010102], respectively). Close the save file in the hex editor.
	Update the save file with this utility.


Website
-----------

	https://github.com/ike9000e/ngii-save-update-util
