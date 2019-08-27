.so Tmac.mm-etc
.if t .Newcentury-fonts
.INITR* \n[.F]
.SIZE 12 14
.ds QUARTER Summer\~2018
.ds SUFFIX u18
.TITLE CMPS-109 \*[QUARTER] Lab\~0 "Unix, C++, Makefile"
.RCS "$Id: lab0-intro-unix.mm,v 1.36 2018-06-19 15:15:57-07 - - $"
.PWD
.URL
.nr SHELLCNT 0 1
.de COMMAND
.   P
.   ft CR
.   nop -bash-\\n+[SHELLCNT]$
.   ft CB
.   nop \\$*
.   ft CR
.   br
.   nf
.   pso \\$* | expand | groffify | fmt -80 -s
.   br
.   ft R
.   fi
..
This lab
will not be graded and there
is no credit for it.
However, it is important you understand the Unix command
line and submit procedure before you actually use it in the
first assignment.
.P
Make sure you know how to submit files.
Submit these files to
.V= lab0 .
Verify that the submit actually worked by looking
in the submit directory for your username.
The
.V= find (1)
command is very useful for things like this.
.P
The submit command copies your files into a directory
.VTCODE* 1 /afs/cats.ucsc.edu/class/cmps109-wm.\*[SUFFIX]/lab0/\[Do]USER
and prefixes each file with a sequence number.
You can see the names of the files, 
but not their contents.
The actual syntax of the 
.V= submit
command is\(::
.VTCODE* 1 "submit cmps109-wm.\*[SUFFIX] lab0 \fIfiles\|.\|.\|.\fP"
.P
.S +2 +2
.UL "\f[BI]Prior experience with Unix is assumed.\f[P]"
.P
.S -2 -1
It is assumed that you know the following\(::
.ALX \[bu] 0 "" 0 0
.LI
The C language,
from a prerequisite course.
.LI
Object-oriented programming, classes, and inheritance (probably Java).
.LI
General use of Linux commands,
and specifically
.V= submit
and
.V= checksource .
.LI
Use of environment variables and setting the
.V= \[Do]PATH
variable.
.LI
Construction and use of
.V= Makefile s.
.LI
A code archival system such as
.V= rcs ,
.V= cvs ,
.V= svn ,
.V= git ,
etc.
.LE
.P
Attend a lab section to ask questions if you don't
understand how submit and the Unix command line in general works.
The TA can explain such things.
Also read the submit checklist\(::
.V= Syllabus/submit-checklist .
.P
Be sure that any textbook or other reference to C++
includes at least C++11 on its cover documentation,
and preferably C++14,
or possibly C++17.
Books that discuss only C++98 should be considered obsolete.
Whenever you need to look up information about C++,
use Google,
or look at
The C++ Resources Network\(::
.V= http://www.cplusplus.com/ .
.P
If you are doing development on your own computer,
make sure that you are using the latest version of
.V= g++
or a newer version.
Make sure that any code you develop compiles and runs on
.V= unix.ucsc.edu ,
since that is the only environment the graders will be using.
If you are developing on your own system,
make sure that the compiler you are using is C++14 compliant.
.COMMAND g++ --version | grep -i g++
.P
The subdirectory
.V= code/
shows a main program and a hello class and
illustrates various features of
C++11, C++14, and C++17 that will be covered later in the course.
.FINISH
