# WildLIFE
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

