************
WiiMU v0.1a4
************

----------
What's new
----------

0.1a4:
* Added Configuration Modification! Thanks tona for sysconf.c!
* Added option to dump without ECC for use with Zestig.
* Translated most menus to the 7 Wii languages.
* Allows you to choose which IOS to load games with.
* Code cleanup.
* Adds GameCube game support. Now Launch Game will load both Wii and Gamecube games!
* TODO: Adds built in updater.

0.1a3:
* Lots of stuff this release!
* Fixes the bug which caused certain users to not get channel lists.
* Adds a Save Manager which currently lists the save files. Copying currently does not work.
* Adds a configuration menu, which, at this current point only displays the configuration.
* Cleans up a lot of code.
* Lots of thanks to crediar, who worked on a lot of stuff while I was taking a break from the scene.

0.1a2:
* Fixes a bug in which PAL users would be switched to NTSC
* (Hopefully) Fixes a bug which cause users of Firmwares other than 3.2U to not be able to grab the channel list.
* Adds a main menu.
* Adds a NAND Dumper (Thanks Redbird!)
* Adds some wiilight functions.

0.1a:
* First release.
* Uses Wiimote.
* Launches Channels through a text interface.

-----------
How to use
-----------

Move: Wiimote D-Pad.
Select: A.
Return: B.

--------------------
What to look out for
--------------------

If you can't get a list of channels, please:
* Tell me which firmware you are on.
* Give me a list of all the files in your /title/ directory recursively.
* Give me a TMD from a subdirectory in /title/00010001/ directory.

------------
What to come
------------

* Music
* Random stuff I think of as I go.

-----
Other
-----

Huge thanks to crediar, who, as you may know if you visit #wiidev regularly, took over this project while I was
taking a break from the Wii Homebrew scene. I may consider him helping me more :)
More thanks to tona, who wrote sysconf.c, which I use extensively in the configuration menu.
Thanks to Davyg2 who wrote libwiikeyboard, which I use for USB Keyboard text input. (If only I could use it correctly =/)
Thanks to emu_kidid for releasing the source code to GCBooter, so I can add it to this release!
Thanks to AlexLH for writing the updater library which is used in this application.
Thanks to Muzer who was always there to help me test.

-------
Credits
-------

Code:		SquidMan, crediar (Took over while I was away!), tona (Wrote sysconf.c), bushing, marcan, Mupen64GC Team (looked at their ISFS code for help), Bool (code from the Wiilight demo), Redbird (NAND Dumper), emu_kidid (GameCube Game Code), AlexLH (Updater library), e1000 (Choosing GC or Wii Loader)
Graphics code:	SquidMan (Good ol' printf goodness!)
Testers:	Muzer
HBC Icon:	LowLines (modified his WAD Manager Icon to just have the channels :P)
ReadMe Base:	DesktopMan. I just took his Tetris ReadMe and changed it, lol.

---------
Thanks to
---------

#wiidev @ EFnet

------------------------------
Alex Marshall / SquidMan
SquidMan72@ [at} gmail
