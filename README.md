I had the basic setup done on the one of my computers.
Now using Mint - I had done previous work in Ununtu.
I had probably unrelated problems in Ubuntu.
LOTS of errors Compiling and Linking.
Basically it came down to
1) Replacing a function for reporting system errors (gone from language).
2) Made changes to Makefile to not use ar and make it simpler.
(And faster by really using as Makefiles are intended to work).
3) There was difference in how globals and externs work. And initialized.
Already had them in one .h file. Added EXTERN which I set to extern for all except the main file.
5) Added function in main file to initialize all that had ben initialized
in declarations.
6) I had to create a bin directory in my home directory and also
7) copy the local life files to the home directory.
8) I added to my ee script entries for editing the Makefile and
9) copying the local files
10 )There is an extensive Test Suite and once I had it running,
it passed all except one test. I have not investigated this yet.
11) You MUST make SURE that FONTS are installed for X-Winows
12) comand is wild_life for non-x version
13) wild_life_X for x version.
14) Makefile makes both versions without any changes.
15) Although I added MINT define when PORTING, it should be KEPT for Ubuntu.
Still Investigate the one error. 

I now have a couple computers I can experiment with.
I started with a fresh install of Ubuntu Linux 20.04

I found the following prerquisites.
I installed emacs (GUI) using Ubuntu Software.
I added "export EDITOR=emacs" to .profile
(you could use a different editor)
Must reboot to take effect.

create directories "bin" and "src" under home directory.
create directory "WildLIFE" under "src"

You will need to following if you do not already
have them.

sudo apt install make

sudo apt install gcc

sudo apt install libx11-dev

sudo apt install csh

Get WildLIFE from github

https://github.com/djdarland/WildLIFE

Under Code
Download ZIP (WildLIFE-master.zip)

Move to src/WildLIFE & extract

change directory to
~/src/WildLIFE/WildLIFE-master/Life/Source

run command
./cp_local.sh

run
./ee

choose 111 to compile

choose 116 to run test suite.

There are some opts that require more software,
but are not required for WildLIFE itself.

WildLIFE

3/8/2021
I noticed and corrected 2 warnings in parser.c and print.c
I added script file ./ee I like to use for most things.
It has menus.
(it has subscripts also).
I added historical files. (useful to understand how I made changes.)

PREVIOUSLY
Having got a new (much faster computer) I cloned my WildLIFE code.
1st time I had done that.
I had a few things I needed to do.
I am using Ubuntu 20.04
I had to istall the c shell (csh)
sudo apt install tcsh
I had to set default editor
in .profile I added
export EDITOR=emacs
there were some missing work directories
I added
--------------------
chmod u+x editdiffs.sh
if (! -d "ERR" ) then 
    mkdir ERR
endif
if (! -d "OUT" ) then 
    mkdir OUT
endif
if (! -d "ERRDIFF" ) then 
    mkdir ERRDIFF
endif
if (! -d "REFDIFF" ) then 
    mkdir REFDIFF
endif
------------------------
in 
check_edit_all
check_edit_all-1.02
check_edit_all-X
-------------------------
I had the wromg path for
prefix		= ~/life_local_redo
in Makefile.102

I forgot to run
cp_local.sh

I forgot How to run X Windows Demo.
Correct way is
in Wildlife/life_local_xxxx_redo2
wild_life_X
load("life_demo")?
demo?

X sometimes has "fatal" errors.
Mostly on exit if clicking upper right hand corner.
I may not pursue.
My interests are not in X and I have little knowledge of it.
-----------------------------------------------------
Plans and Goals
* There is an easy install method. [see Life/INSTALL] that is:
* tar -xzvf WildLIFE..[version]...tar.gz
*  THEN in Life/Source subdirectory
* 
*  do
*   ./mk.sh to make most current version
*   ./mk-1.02.sh  to make version more compatible with 1.02 Test Suite
*   ./mk-X.sh     to make version with X Windows support compiled in
*   
*    Also
*       ./cp_local.sh   to copy local directories for all versions.
*
* There are prerequisites.
    * to run test_suite ./ts.sh
    * for original 1.02 suite use ./ts-1.02.sh
    * for with X Windows enabled ./ts-X.sh
* X Windows now getting events.
* There are some problems with X-Windows.
  * Main thing is to make sure that you have the needed fonts.
  * I am not personally using X-Windows - I have done all I know how
  * In life_local_redo2/Examples queens.lf works - but not a good font
  * I was just using a font I knew existed.
  * flowers works somewhat - but also get fatal errors at points.
  * in life_local_redo2/Demo life_demo responds to events - things
  * it calls are not there yet

  * Main problem I I had that caused not getting events was
  * using a .wild_life file to start applications.
  * I found that out by accident!!!

  * Completed getting X Windows working
  * Also works with cygwin

  * Went back to test suites -- See Source/Port.txt


---------------------------------------------------------------------------
Copyright (C) Digital Equipment Corporation 1994
This work may not be copied or reproduced in whole or in part or for any
commercial purpose. Permission to copy in whole or in part without payment of
fee is granted for non-profit educational and research purposes provided that
all such whole or partial copies include the following: a notice that such
copying is by permission of the Paris Research Laboratory of Digital Equipment
Centre Technique Europe, in Rueil-Malmaison, France; an acknowledgement of the
authors and individual contributors to the work; and all applicable portions
of the copyright notice. Copying, reproducing, or republishing for any other
purpose shall require a license with payment of fee to the Paris Research
Laboratory. All rights reserved.

I have been informed that DEC was bought by Compaq in 1998.

And that Compaq was bought by HP in 2002.

I tried to contact HP, but got no response.

See also Life/LICENSE

---------------------------------------------------------------------------

I am only using for for non-profit educational and research purposes.

My prior work is at:

https://dennisdarland.com/my_sw_projects/index.html#LIFE

I have used WildLIFE in research in philosophical logic:

https://dennisdarland.com/my_sw_projects/index.html#DarlandPhilosophy

My philosophy is described more at:

https://eclectic-philosopher.com/e_pluribus_unum/1951-ad-dennis-joe-darland/

