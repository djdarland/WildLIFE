;;; wild_life-mode.el --- major mode for Wild_Life-Mode  -*- lexical-binding:t -*-

;; Copyright (C) 1986-1987, 1997-1999, 2002-2003, 2011-2015 Free
;; Software Foundation, Inc.

;; Authors: Emil Åström <emil_astrom(at)hotmail(dot)com>
;;          Milan Zamazal <pdm(at)freesoft(dot)cz>
;;          Stefan Bruda <stefan(at)bruda(dot)ca>

;; Wild_Life modifications Dennis J. Darland <pal(at)dennisdarland(dot)com>

;;          * See below for more details
;; Maintainer: Stefan Bruda <stefan(at)bruda(dot)ca>
;; Keywords: wild_life-mode

(defvar wild_life-mode-version "2.07"
  "Wild_Life-Mode mode version number.")

;; This file is part of GNU Emacs.

;; GNU Emacs is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; GNU Emacs is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU Emacs.  If not, see <http://www.gnu.org/licenses/>.

;; Original author: Masanobu UMEDA <umerin(at)mse(dot)kyutech(dot)ac(dot)jp>
;; Parts of this file was taken from a modified version of the original
;; by Johan Andersson, Peter Olin, Mats Carlsson, Johan Bevemyr, Stefan
;; Andersson, and Per Danielsson (all SICS people), and Henrik Båkman
;; at Uppsala University, Sweden.
;;
;; Some ideas and also a few lines of code have been borrowed (not stolen ;-)
;; from Oz.el, the Emacs major mode for the Oz programming language,
;; Copyright (C) 1993 DFKI GmbH, Germany, with permission.
;; Authored by Ralf Scheidhauer and Michael Mehl
;;   ([scheidhr|mehl](at)dfki(dot)uni-sb(dot)de)
;;
;; More ideas and code have been taken from the SICStus debugger mode
;; (http://www.csd.uu.se/~perm/source_debug/index.shtml -- broken link
;; as of Mon May 5 08:23:48 EDT 2003) by Per Mildner.
;;
;; Additions for ECLiPSe and other helpful suggestions: Stephan Heuel
;; <heuel(at)ipb(dot)uni-bonn(dot)de>

;;; Commentary:
;;
;; This package provides a major mode for editing Wild_Life-Mode code, with
;; all the bells and whistles one would expect, including syntax
;; highlighting and auto indentation.  It can also send regions to an
;; inferior Wild_Life-Mode process.
;;
;; The code requires the comint, easymenu, info, imenu, and font-lock
;; libraries.  These are normally distributed with GNU Emacs and
;; XEmacs.

;;; Installation:
;;
;; Insert the following lines in your init file: (probably .gnu-emacs-custom)
;;
;;
;; (setq load-path (cons "~/LifeEmacs_Mod" load-path))
;; (setq wild_life-system 'swi)
;; (setq auto-mode-alist (cons '("\\.lf" . wild_life-mode) auto-mode-alist))
;; (setq auto-mode-alist (cons '("\\.in" . wild_life-mode) auto-mode-alist))
;; (autoload 'run-wild_life "wild_life" "Start a WildLife sub-process." t)
;; (autoload 'wild_life-mode "wild_life-mode" "Major mode for editing Wild Life programs." t)






;; where the path in the first line is the file system path to this file.
;; MSDOS paths can be written like "d:/programs/emacs-19.34/site-lisp".
;; Note: In XEmacs, either `/usr/lib/xemacs/site-lisp' (RPM default in
;; Red Hat-based distributions) or `/usr/local/lib/xemacs/site-lisp'
;; (default when compiling from sources) are automatically added to
;; `load-path', so the first line is not necessary provided that you
;; put this file in the appropriate place.
;;
;; The last s-expression above makes sure that files ending with .pl
;; are assumed to be Wild_Life-Mode files and not Perl, which is the default
;; Emacs setting.  If this is not wanted, remove this line.  It is then
;; necessary to either
;;
;;  o  insert in your Wild_Life-Mode files the following comment as the first line:
;;
;;       % -*- Mode: Wild_Life-Mode -*-
;;
;;     and then the file will be open in Wild_Life-Mode mode no matter its
;;     extension, or
;;
;;  o  manually switch to wild_life-mode mode after opening a Wild_Life-Mode file, by typing
;;     M-x wild_life-mode.
;;
;; If the command to start the wild_life-mode process ('sicstus', 'pl' or
;; 'swipl' for SWI wild_life-mode, etc.) is not available in the default path,
;; then it is necessary to set the value of the environment variable
;; EWILD_LIFE-MODE to a shell command to invoke the wild_life-mode process.  In XEmacs
;; and Emacs 20+ you can also customize the variable
;; `wild_life-mode-program-name' (in the group `wild_life-mode-inferior') and provide
;; a full path for your Wild_Life-Mode system (swi, scitus, etc.).
;;
;; Note: I (Stefan, the current maintainer) work under XEmacs.  Future
;;   developments will thus be biased towards XEmacs (OK, I admit it,
;;   I am biased towards XEmacs in general), though I will do my best
;;   to keep the GNU Emacs compatibility.  So if you work under Emacs
;;   and see something that does not work do drop me a line, as I have
;;   a smaller chance to notice this kind of bugs otherwise.

;; Changelog:

;; Version 2.07
;; Created wild_life-mode.el from prolog.el
;; globally changed prolog.el to wild_life-mode.el
;; globally changed prolog to wild_life-mode
;; globally changed wild_life-mode-mode to wild_life-mode
;; am using 'swi as I am more familiar with it (have used it)
;; Removed "SWI" in some places
;; changed suffix from .pl to .lf
;; changed executable names from swi & pl to wild_life & wl
;; changed reconsult to load & "." there to "?"
;; having trouble with ' in load instead of "
;; will go back to that later
;; indenting & syntax highlighting seem to basically work.
;; no doubt syntax highighting can be improved
;; there are no /* --- */ comments in wild_life -- but there were in swi
;; that is not fixed yet
;; no doubt other problems exist




;; Version 1.22:
;;  o  Allowed both 'swipl' and 'pl' as names for the SWI Wild_Life-Mode
;;     interpreter.
;;  o  Atoms that start a line are not blindly colored as
;;     predicates.  Instead we check that they are followed by ( or
;;     :- first.  Patch suggested by Guy Wiener.
;; Version 1.21:
;;  o  Cleaned up the code that defines faces.  The missing face
;;     warnings on some Emacsen should disappear.
;; Version 1.20:
;;  o  Improved the handling of clause start detection and multi-line
;;     comments: `wild_life-mode-clause-start' no longer finds non-predicate
;;     (e.g., capitalized strings) beginning of clauses.
;;     `wild_life-mode-tokenize' recognizes when the end point is within a
;;     multi-line comment.
;; Version 1.19:
;;  o  Minimal changes for Aquamacs inclusion and in general for
;;     better coping with finding the Wild_Life-Mode executable.  Patch
;;     provided by David Reitter
;; Version 1.18:
;;  o  Fixed syntax highlighting for clause heads that do not begin at
;;     the beginning of the line.
;;  o  Fixed compilation warnings under Emacs.
;;  o  Updated the email address of the current maintainer.
;; Version 1.17:
;;  o  Minor indentation fix (patch by Markus Triska)
;;  o  `wild_life-mode-underscore-wordchar-flag' defaults now to nil (more
;;     consistent to other Emacs modes)
;; Version 1.16:
;;  o  Eliminated a possible compilation warning.
;; Version 1.15:
;;  o  Introduced three new customizable variables: electric colon
;;     (`wild_life-mode-electric-colon-flag', default nil), electric dash
;;     (`wild_life-mode-electric-dash-flag', default nil), and a possibility
;;     to prevent the predicate template insertion from adding commas
;;     (`wild_life-mode-electric-dot-full-predicate-template', defaults to t
;;     since it seems quicker to me to just type those commas).  A
;;     trivial adaptation of a patch by Markus Triska.
;;  o  Improved the behavior of electric if-then-else to only skip
;;     forward if the parenthesis/semicolon is preceded by
;;     whitespace.  Once more a trivial adaptation of a patch by
;;     Markus Triska.
;; Version 1.14:
;;  o  Cleaned up align code.  `wild_life-mode-align-flag' is eliminated (since
;;     on a second thought it does not do anything useful).  Added key
;;     binding (C-c C-a) and menu entry for alignment.
;;  o  Condensed regular expressions for lower and upper case
;;     characters (GNU Emacs seems to go over the regexp length limit
;;     with the original form).  My code on the matter was improved
;;     considerably by Markus Triska.
;;  o  Fixed `wild_life-mode-insert-spaces-after-paren' (which used an
;;     uninitialized variable).
;;  o  Minor changes to clean up the code and avoid some implicit
;;     package requirements.
;; Version 1.13:
;;  o  Removed the use of `map-char-table' in `wild_life-mode-build-case-strings'
;;     which appears to cause problems in (at least) Emacs 23.0.0.1.
;;  o  Added if-then-else indentation + corresponding electric
;;     characters.  New customization: `wild_life-mode-electric-if-then-else-flag'
;;  o  Align support (requires `align').  New customization:
;;     `wild_life-mode-align-flag'.
;;  o  Temporary consult files have now the same name throughout the
;;     session.  This prevents issues with reconsulting a buffer
;;     (this event is no longer passed to Wild_Life-Mode as a request to
;;     consult a new file).
;;  o  Adaptive fill mode is now turned on.  Comment indentation is
;;     still worse than it could be though, I am working on it.
;;  o  Improved filling and auto-filling capabilities.  Now block
;;     comments should be [auto-]filled correctly most of the time;
;;     the following pattern in particular is worth noting as being
;;     filled correctly:
;;         <some code here> % some comment here that goes beyond the
;;                          % rightmost column, possibly combined with
;;                          % subsequent comment lines
;;  o  `wild_life-mode-char-quote-workaround' now defaults to nil.
;;  o  Note: Many of the above improvements have been suggested by
;;     Markus Triska, who also provided useful patches on the matter
;;     when he realized that I was slow in responding.  Many thanks.
;; Version 1.11 / 1.12
;;  o  GNU Emacs compatibility fix for paragraph filling (fixed
;;     incorrectly in 1.11, fix fixed in 1.12).
;; Version 1.10
;;  o  Added paragraph filling in comment blocks and also correct auto
;;     filling for comments.
;;  o  Fixed the possible "Regular expression too big" error in
;;     `wild_life-mode-electric-dot'.
;; Version 1.9
;;  o  Parenthesis expressions are now indented by default so that
;;     components go one underneath the other, just as for compound
;;     terms.  You can use the old style (the second and subsequent
;;     lines being indented to the right in a parenthesis expression)
;;     by setting the customizable variable `wild_life-mode-paren-indent-p'
;;     (group "Wild_Life-Mode Indentation") to t.
;;  o  (Somehow awkward) handling of the 0' character escape
;;     sequence.  I am looking into a better way of doing it but
;;     prospects look bleak.  If this breaks things for you please let
;;     me know and also set the `wild_life-mode-char-quote-workaround' (group
;;     "Wild_Life-Mode Other") to nil.
;; Version 1.8
;;  o  Key binding fix.
;; Version 1.7
;;  o  Fixed a number of issues with the syntax of single quotes,
;;     including Debian bug #324520.
;; Version 1.6
;;  o  Fixed mercury mode menu initialization (Debian bug #226121).
;;  o  Fixed (i.e., eliminated) Delete remapping (Debian bug #229636).
;;  o  Corrected indentation for clauses defining quoted atoms.
;; Version 1.5:
;;  o  Keywords fontifying should work in console mode so this is
;;     enabled everywhere.
;; Version 1.4:
;;  o  Now supports GNU Wild_Life-Mode--minor adaptation of a patch by Stefan
;;     Moeding.
;; Version 1.3:
;;  o  Info-follow-nearest-node now called correctly under Emacs too
;;     (thanks to Nicolas Pelletier).  Should be implemented more
;;     elegantly (i.e., without compilation warnings) in the future.
;; Version 1.2:
;;  o  Another prompt fix, still in SWI mode (people seem to have
;;     changed the prompt of SWI Wild_Life-Mode).
;; Version 1.1:
;;  o  Fixed dots in the end of line comments causing indentation
;;     problems.  The following code is now correctly indented (note
;;     the dot terminating the comment):
;;        a(X) :- b(X),
;;            c(X).                  % comment here.
;;        a(X).
;;     and so is this (and variants):
;;        a(X) :- b(X),
;;            c(X).                  /* comment here.  */
;;        a(X).
;; Version 1.0:
;;  o  Revamped the menu system.
;;  o  Yet another prompt recognition fix (SWI mode).
;;  o  This is more of a renumbering than a new edition.  I promoted
;;     the mode to version 1.0 to emphasize the fact that it is now
;;     mature and stable enough to be considered production (in my
;;     opinion anyway).
;; Version 0.1.41:
;;  o  GNU Emacs compatibility fixes.
;; Version 0.1.40:
;;  o  wild_life-mode-get-predspec is now suitable to be called as
;;     imenu-extract-index-name-function.  The predicate index works.
;;  o  Since imenu works now as advertised, wild_life-mode-imenu-flag is t
;;     by default.
;;  o  Eliminated wild_life-mode-create-predicate-index since the imenu
;;     utilities now work well.  Actually, this function is also
;;     buggy, and I see no reason to fix it since we do not need it
;;     anyway.
;;  o  Fixed wild_life-mode-pred-start, wild_life-mode-clause-start, wild_life-mode-clause-info.
;;  o  Fix for wild_life-mode-build-case-strings; now wild_life-mode-upper-case-string
;;     and wild_life-mode-lower-case-string are correctly initialized,
;;  o  Various font-lock changes; most importantly, block comments (/*
;;     ... */) are now correctly fontified in XEmacs even when they
;;     extend on multiple lines.
;; Version 0.1.36:
;;  o  The debug prompt of SWI Wild_Life-Mode is now correctly recognized.
;; Version 0.1.35:
;;  o  Minor font-lock bug fixes.

;;; TODO:

;; Replace ":type 'sexp" with more precise Custom types.

;;; Code:

(require 'comint)

(eval-when-compile
  (require 'font-lock)
  ;; We need imenu everywhere because of the predicate index!
  (require 'imenu)
  ;)
  (require 'shell)
  )

(require 'easymenu)
(require 'align)


(defgroup wild_life-mode nil
  "Editing and running Wild_Life-Mode and Mercury files."
  :group 'languages)

(defgroup wild_life-mode-faces nil
  "Wild_Life-Mode mode specific faces."
  :group 'font-lock)

(defgroup wild_life-mode-indentation nil
  "Wild_Life-Mode mode indentation configuration."
  :group 'wild_life-mode)

(defgroup wild_life-mode-font-lock nil
  "Wild_Life-Mode mode font locking patterns."
  :group 'wild_life-mode)

(defgroup wild_life-mode-keyboard nil
  "Wild_Life-Mode mode keyboard flags."
  :group 'wild_life-mode)

(defgroup wild_life-mode-inferior nil
  "Inferior Wild_Life-Mode mode options."
  :group 'wild_life-mode)

(defgroup wild_life-mode-other nil
  "Other Wild_Life-Mode mode options."
  :group 'wild_life-mode)


;;-------------------------------------------------------------------
;; User configurable variables
;;-------------------------------------------------------------------

;; General configuration

(defcustom wild_life-mode-system nil
  "Wild_Life-Mode interpreter/compiler used.
The value of this variable is nil or a symbol.
If it is a symbol, it determines default values of other configuration
variables with respect to properties of the specified Wild_Life-Mode
interpreter/compiler.

Currently recognized symbol values are:
eclipse - Eclipse Wild_Life-Mode
mercury - Mercury
sicstus - SICStus Wild_Life-Mode
swi     - Wild_Life-Mode
gnu     - GNU Wild_Life-Mode"
  :version "24.1"
  :group 'wild_life-mode
  :type '(choice (const :tag "SICStus" :value sicstus)
                 (const :tag "Wild_Life-Mode" :value swi)
                 (const :tag "GNU Wild_Life-Mode" :value gnu)
                 (const :tag "ECLiPSe Wild_Life-Mode" :value eclipse)
                 ;; Mercury shouldn't be needed since we have a separate
                 ;; major mode for it.
                 (const :tag "Default" :value nil)))
(make-variable-buffer-local 'wild_life-mode-system)

;; NB: This alist can not be processed in wild_life-mode-variables to
;; create a wild_life-mode-system-version-i variable since it is needed
;; prior to the call to wild_life-mode-variables.
(defcustom wild_life-mode-system-version
  '((sicstus  (3 . 6))
    (swi      (0 . 0))
    (mercury  (0 . 0))
    (eclipse  (3 . 7))
    (gnu      (0 . 0)))
  ;; FIXME: This should be auto-detected instead of user-provided.
  "Alist of Wild_Life-Mode system versions.
The version numbers are of the format (Major . Minor)."
  :version "24.1"
  :type '(repeat (list (symbol :tag "System")
                       (cons :tag "Version numbers" (integer :tag "Major")
                             (integer :tag "Minor"))))
  :group 'wild_life-mode)

;; Indentation

(defcustom wild_life-mode-indent-width 4
  "The indentation width used by the editing buffer."
  :group 'wild_life-mode-indentation
  :type 'integer)

(defcustom wild_life-mode-left-indent-regexp "\\(;\\|\\*?->\\)"
  "Regexp for `wild_life-mode-electric-if-then-else-flag'."
  :version "24.1"
  :group 'wild_life-mode-indentation
  :type 'regexp)

(defcustom wild_life-mode-paren-indent-p nil
  "If non-nil, increase indentation for parenthesis expressions.
The second and subsequent line in a parenthesis expression other than
a compound term can either be indented `wild_life-mode-paren-indent' to the
right (if this variable is non-nil) or in the same way as for compound
terms (if this variable is nil, default)."
  :version "24.1"
  :group 'wild_life-mode-indentation
  :type 'boolean)

(defcustom wild_life-mode-paren-indent 4
  "The indentation increase for parenthesis expressions.
Only used in ( If -> Then ; Else) and ( Disj1 ; Disj2 ) style expressions."
  :version "24.1"
  :group 'wild_life-mode-indentation
  :type 'integer)

(defcustom wild_life-mode-parse-mode 'beg-of-clause
  "The parse mode used (decides from which point parsing is done).
Legal values:
'beg-of-line   - starts parsing at the beginning of a line, unless the
                 previous line ends with a backslash.  Fast, but has
                 problems detecting multiline /* */ comments.
'beg-of-clause - starts parsing at the beginning of the current clause.
                 Slow, but copes better with /* */ comments."
  :version "24.1"
  :group 'wild_life-mode-indentation
  :type '(choice (const :value beg-of-line)
                 (const :value beg-of-clause)))

;; Font locking

(defcustom wild_life-mode-keywords
  '((eclipse
     ("use_module" "begin_module" "module_interface" "dynamic"
      "external" "export" "dbgcomp" "nodbgcomp" "compile"))
    (mercury
     ("all" "else" "end_module" "equality" "external" "fail" "func" "if"
      "implementation" "import_module" "include_module" "inst" "instance"
      "interface" "mode" "module" "not" "pragma" "pred" "some" "then" "true"
      "type" "typeclass" "use_module" "where"))
    (sicstus
     ("block" "dynamic" "mode" "module" "multifile" "meta_predicate"
      "parallel" "public" "sequential" "volatile"))
    (swi
     ("discontiguous" "dynamic" "ensure_loaded" "export" "export_list" "import"
      "meta_predicate" "module" "module_transparent" "multifile" "require"
      "use_module" "volatile"))
    (gnu
     ("built_in" "char_conversion" "discontiguous" "dynamic" "ensure_linked"
      "ensure_loaded" "foreign" "include" "initialization" "multifile" "op"
      "public" "set_wild_life-mode_flag"))
    (t
     ;; FIXME: Shouldn't we just use the union of all the above here?
     ("dynamic" "module")))
  "Alist of Wild_Life-Mode keywords which is used for font locking of directives."
  :version "24.1"
  :group 'wild_life-mode-font-lock
  :type 'sexp)

(defcustom wild_life-mode-types
  '((mercury
     ("char" "float" "int" "io__state" "string" "univ"))
    (t nil))
  "Alist of Wild_Life-Mode types used by font locking."
  :version "24.1"
  :group 'wild_life-mode-font-lock
  :type 'sexp)

(defcustom wild_life-mode-specificators
  '((mercury
     ("bound" "di" "free" "ground" "in" "mdi" "mui" "muo" "out" "ui" "uo"))
    (t nil))
  "Alist of Wild_Life-Mode mode specificators used by font locking."
  :version "24.1"
  :group 'wild_life-mode-font-lock
  :type 'sexp)

(defcustom wild_life-mode-determinism-specificators
  '((mercury
     ("cc_multi" "cc_nondet" "det" "erroneous" "failure" "multi" "nondet"
      "semidet"))
    (t nil))
  "Alist of Wild_Life-Mode determinism specificators used by font locking."
  :version "24.1"
  :group 'wild_life-mode-font-lock
  :type 'sexp)

(defcustom wild_life-mode-directives
  '((mercury
     ("^#[0-9]+"))
    (t nil))
  "Alist of Wild_Life-Mode source code directives used by font locking."
  :version "24.1"
  :group 'wild_life-mode-font-lock
  :type 'sexp)


;; Keyboard

(defcustom wild_life-mode-hungry-delete-key-flag nil
  "Non-nil means delete key consumes all preceding spaces."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-electric-dot-flag nil
  "Non-nil means make dot key electric.
Electric dot appends newline or inserts head of a new clause.
If dot is pressed at the end of a line where at least one white space
precedes the point, it inserts a recursive call to the current predicate.
If dot is pressed at the beginning of an empty line, it inserts the head
of a new clause for the current predicate.  It does not apply in strings
and comments.
It does not apply in strings and comments."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-electric-dot-full-predicate-template nil
  "If nil, electric dot inserts only the current predicate's name and `('
for recursive calls or new clause heads.  Non-nil means to also
insert enough commas to cover the predicate's arity and `)',
and dot and newline for recursive calls."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-electric-underscore-flag nil
  "Non-nil means make underscore key electric.
Electric underscore replaces the current variable with underscore.
If underscore is pressed not on a variable then it behaves as usual."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-electric-if-then-else-flag nil
  "Non-nil makes `(', `>' and `;' electric
to automatically indent if-then-else constructs."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-electric-colon-flag nil
  "Makes `:' electric (inserts `:-' on a new line).
If non-nil, pressing `:' at the end of a line that starts in
the first column (i.e., clause heads) inserts ` :-' and newline."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-electric-dash-flag nil
  "Makes `-' electric (inserts a `-->' on a new line).
If non-nil, pressing `-' at the end of a line that starts in
the first column (i.e., DCG heads) inserts ` -->' and newline."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

(defcustom wild_life-mode-old-sicstus-keys-flag nil
  "Non-nil means old SICStus Wild_Life-Mode mode keybindings are used."
  :version "24.1"
  :group 'wild_life-mode-keyboard
  :type 'boolean)

;; Inferior mode

(defcustom wild_life-mode-program-name
  `(((getenv "EWILD_LIFE-MODE") (eval (getenv "EWILD_LIFE-MODE")))
    (eclipse "eclipse")
    (mercury nil)
    (sicstus "sicstus")
    (swi ,(if (not (executable-find "wl")) "wl" "wild_life"))
    (gnu "gwild_life-mode")
    (t ,(let ((names '("wild_life-mode" "gwild_life-mode" "wild_life" "wl")))
 	  (while (and names
 		      (not (executable-find (car names))))
 	    (setq names (cdr names)))
 	  (or (car names) "wild_life-mode"))))
  "Alist of program names for invoking an inferior Wild_Life-Mode with `run-wild_life-mode'."
  :group 'wild_life-mode-inferior
  :type 'sexp)
(defun wild_life-mode-program-name ()
  (wild_life-mode-find-value-by-system wild_life-mode-program-name))

(defcustom wild_life-mode-program-switches
  '((sicstus ("-i"))
    (t nil))
  "Alist of switches given to inferior Wild_Life-Mode run with `run-wild_life-mode'."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'sexp)
(defun wild_life-mode-program-switches ()
  (wild_life-mode-find-value-by-system wild_life-mode-program-switches))

(defcustom wild_life-mode-consult-string
  '((eclipse "[%f].")
    (mercury nil)
    (sicstus (eval (if (wild_life-mode-atleast-version '(3 . 7))
                       "wild_life-mode:zap_file(%m,%b,consult,%l)."
                     "wild_life-mode:zap_file(%m,%b,consult).")))
    (swi "[%f].")
    (gnu     "[%f].")
    (t "load(%f)?"))
  "Alist of strings defining predicate for reconsulting.

Some parts of the string are replaced:
`%f' by the name of the consulted file (can be a temporary file)
`%b' by the file name of the buffer to consult
`%m' by the module name and name of the consulted file separated by colon
`%l' by the line offset into the file.  This is 0 unless consulting a
     region of a buffer, in which case it is the number of lines before
     the region."
  :group 'wild_life-mode-inferior
  :type 'sexp)
(defun wild_life-mode-consult-string ()
  (wild_life-mode-find-value-by-system wild_life-mode-consult-string))

(defcustom wild_life-mode-compile-string
  '((eclipse "[%f].")
    (mercury "mmake ")
    (sicstus (eval (if (wild_life-mode-atleast-version '(3 . 7))
                       "wild_life-mode:zap_file(%m,%b,compile,%l)."
                     "wild_life-mode:zap_file(%m,%b,compile).")))
    (swi "[%f].")
    (t "compile(%f)."))
  "Alist of strings and lists defining predicate for recompilation.

Some parts of the string are replaced:
`%f' by the name of the compiled file (can be a temporary file)
`%b' by the file name of the buffer to compile
`%m' by the module name and name of the compiled file separated by colon
`%l' by the line offset into the file.  This is 0 unless compiling a
     region of a buffer, in which case it is the number of lines before
     the region.

If `wild_life-mode-program-name' is non-nil, it is a string sent to a Wild_Life-Mode process.
If `wild_life-mode-program-name' is nil, it is an argument to the `compile' function."
  :group 'wild_life-mode-inferior
  :type 'sexp)
(defun wild_life-mode-compile-string ()
  (wild_life-mode-find-value-by-system wild_life-mode-compile-string))

(defcustom wild_life-mode-eof-string "end_of_file.\n"
  "Alist of strings that represent end of file for wild_life-mode.
nil means send actual operating system end of file."
  :group 'wild_life-mode-inferior
  :type 'sexp)

(defcustom wild_life-mode-prompt-regexp
  '((eclipse "^[a-zA-Z0-9()]* *\\?- \\|^\\[[a-zA-Z]* [0-9]*\\]:")
    (sicstus "| [ ?][- ] *")
    (swi "^\\(\\[[a-zA-Z]*\\] \\)?[1-9]?[0-9]*[ ]?\\?- \\|^| +")
    (gnu "^| \\?-")
    (t "^|? *\\?-"))
  "Alist of prompts of the wild_life-mode system command line."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'sexp)
(defun wild_life-mode-prompt-regexp ()
  (wild_life-mode-find-value-by-system wild_life-mode-prompt-regexp))

;; (defcustom wild_life-mode-continued-prompt-regexp
;;   '((sicstus "^\\(| +\\|     +\\)")
;;     (t "^|: +"))
;;   "Alist of regexps matching the prompt when consulting `user'."
;;   :group 'wild_life-mode-inferior
;;   :type 'sexp)

(defcustom wild_life-mode-debug-on-string "debug.\n"
  "Predicate for enabling debug mode."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'string)

(defcustom wild_life-mode-debug-off-string "nodebug.\n"
  "Predicate for disabling debug mode."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'string)

(defcustom wild_life-mode-trace-on-string "trace.\n"
  "Predicate for enabling tracing."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'string)

(defcustom wild_life-mode-trace-off-string "notrace.\n"
  "Predicate for disabling tracing."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'string)

(defcustom wild_life-mode-zip-on-string "zip.\n"
  "Predicate for enabling zip mode for SICStus."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'string)

(defcustom wild_life-mode-zip-off-string "nozip.\n"
  "Predicate for disabling zip mode for SICStus."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'string)

(defcustom wild_life-mode-use-standard-consult-compile-method-flag t
  "Non-nil means use the standard compilation method.
Otherwise the new compilation method will be used.  This
utilizes a special compilation buffer with the associated
features such as parsing of error messages and automatically
jumping to the source code responsible for the error.

Warning: the new method is so far only experimental and
does contain bugs.  The recommended setting for the novice user
is non-nil for this variable."
  :version "24.1"
  :group 'wild_life-mode-inferior
  :type 'boolean)


;; Miscellaneous

(defcustom wild_life-mode-imenu-flag t
  "Non-nil means add a clause index menu for all wild_life-mode files."
  :version "24.1"
  :group 'wild_life-mode-other
  :type 'boolean)

(defcustom wild_life-mode-imenu-max-lines 3000
  "The maximum number of lines of the file for imenu to be enabled.
Relevant only when `wild_life-mode-imenu-flag' is non-nil."
  :version "24.1"
  :group 'wild_life-mode-other
  :type 'integer)

(defcustom wild_life-mode-info-predicate-index
  "(sicstus)Predicate Index"
  "The info node for the SICStus predicate index."
  :version "24.1"
  :group 'wild_life-mode-other
  :type 'string)

(defcustom wild_life-mode-underscore-wordchar-flag nil
  "Non-nil means underscore (_) is a word-constituent character."
  :version "24.1"
  :group 'wild_life-mode-other
  :type 'boolean)
(make-obsolete-variable 'wild_life-mode-underscore-wordchar-flag
                        'superword-mode "24.4")

(defcustom wild_life-mode-use-sicstus-sd nil
  "If non-nil, use the source level debugger of SICStus 3#7 and later."
  :version "24.1"
  :group 'wild_life-mode-other
  :type 'boolean)

(defcustom wild_life-mode-char-quote-workaround nil
  "If non-nil, declare 0 as a quote character to handle 0'<char>.
This is really kludgy, and unneeded (i.e. obsolete) in Emacs>=24."
  :version "24.1"
  :group 'wild_life-mode-other
  :type 'boolean)
(make-obsolete-variable 'wild_life-mode-char-quote-workaround nil "24.1")


;;-------------------------------------------------------------------
;; Internal variables
;;-------------------------------------------------------------------

;;(defvar wild_life-mode-temp-filename "")   ; Later set by `wild_life-mode-temporary-file'

(defvar wild_life-mode-syntax-table
  ;; The syntax accepted varies depending on the implementation used.
  ;; Here are some of the differences:
  ;; - SWI-Wild_Life-Mode accepts nested /*..*/ comments.
  ;; - Edinburgh-style Wild_Life-Modes take <radix>'<number> for non-decimal number,
  ;;   whereas ISO-style Wild_Life-Modes use 0[obx]<number> instead.
  ;; - In atoms \x<hex> sometimes needs a terminating \ (ISO-style)
  ;;   and sometimes not.
  (let ((table (make-syntax-table)))
    (modify-syntax-entry ?_ (if wild_life-mode-underscore-wordchar-flag "w" "_") table)
    (modify-syntax-entry ?+ "." table)
    (modify-syntax-entry ?- "." table)
    (modify-syntax-entry ?= "." table)
    (modify-syntax-entry ?< "." table)
    (modify-syntax-entry ?> "." table)
    (modify-syntax-entry ?| "." table)
    (modify-syntax-entry ?\' "\"" table)

    ;; Any better way to handle the 0'<char> construct?!?
    (when (and wild_life-mode-char-quote-workaround
               (not (fboundp 'syntax-propertize-rules)))
      (modify-syntax-entry ?0 "\\" table))

    (modify-syntax-entry ?% "<" table)
    (modify-syntax-entry ?\n ">" table)
    (if (featurep 'xemacs)
        (progn
          (modify-syntax-entry ?* ". 67" table)
          (modify-syntax-entry ?/ ". 58" table)
          )
      ;; Emacs wants to see this it seems:
      (modify-syntax-entry ?* ". 23b" table)
      (modify-syntax-entry ?/ ". 14" table)
      )
    table))

(defconst wild_life-mode-atom-char-regexp
  "[[:alnum:]_$]"
  "Regexp specifying characters which constitute atoms without quoting.")
(defconst wild_life-mode-atom-regexp
  (format "[[:lower:]$]%s*" wild_life-mode-atom-char-regexp))

(defconst wild_life-mode-left-paren "[[({]"     ;FIXME: Why not \\s(?
  "The characters used as left parentheses for the indentation code.")
(defconst wild_life-mode-right-paren "[])}]"    ;FIXME: Why not \\s)?
  "The characters used as right parentheses for the indentation code.")

(defconst wild_life-mode-quoted-atom-regexp
  "\\(^\\|[^0-9]\\)\\('\\([^\n']\\|\\\\'\\)*'\\)"
  "Regexp matching a quoted atom.")
(defconst wild_life-mode-string-regexp
  "\\(\"\\([^\n\"]\\|\\\\\"\\)*\"\\)"
  "Regexp matching a string.")
(defconst wild_life-mode-head-delimiter "\\(:-\\|\\+:\\|-:\\|\\+\\?\\|-\\?\\|-->\\)"
  "A regexp for matching on the end delimiter of a head (e.g. \":-\").")

(defvar wild_life-mode-compilation-buffer "*wild_life-mode-compilation*"
  "Name of the output buffer for Wild_Life-Mode compilation/consulting.")

(defvar wild_life-mode-temporary-file-name nil)
(defvar wild_life-mode-keywords-i nil)
(defvar wild_life-mode-types-i nil)
(defvar wild_life-mode-specificators-i nil)
(defvar wild_life-mode-determinism-specificators-i nil)
(defvar wild_life-mode-directives-i nil)
(defvar wild_life-mode-eof-string-i nil)
;; (defvar wild_life-mode-continued-prompt-regexp-i nil)
(defvar wild_life-mode-help-function-i nil)

(defvar wild_life-mode-align-rules
  (eval-when-compile
    (mapcar
     (lambda (x)
       (let ((name (car x))
             (sym  (cdr x)))
         `(,(intern (format "wild_life-mode-%s" name))
           (regexp . ,(format "\\(\\s-*\\)%s\\(\\s-*\\)" sym))
           (tab-stop . nil)
           (modes . '(wild_life-mode))
           (group . (1 2)))))
     '(("dcg" . "-->") ("rule" . ":-") ("simplification" . "<=>")
       ("propagation" . "==>")))))

;; SMIE support

(require 'smie)

(defun wild_life-mode-smie-forward-token ()
  ;; FIXME: Add support for 0'<char>, if needed after adding it to
  ;; syntax-propertize-functions.
  (forward-comment (point-max))
  (buffer-substring-no-properties
   (point)
   (progn (cond
           ((looking-at "[!;]") (forward-char 1))
           ((not (zerop (skip-chars-forward "#&*+-./:<=>?@\\^`~"))))
           ((not (zerop (skip-syntax-forward "w_'"))))
           ;; In case of non-ASCII punctuation.
           ((not (zerop (skip-syntax-forward ".")))))
          (point))))

(defun wild_life-mode-smie-backward-token ()
  ;; FIXME: Add support for 0'<char>, if needed after adding it to
  ;; syntax-propertize-functions.
  (forward-comment (- (point-max)))
  (buffer-substring-no-properties
   (point)
   (progn (cond
           ((memq (char-before) '(?! ?\;)) (forward-char -1))
           ((not (zerop (skip-chars-backward "#&*+-./:<=>?@\\^`~"))))
           ((not (zerop (skip-syntax-backward "w_'"))))
           ;; In case of non-ASCII punctuation.
           ((not (zerop (skip-syntax-backward ".")))))
          (point))))

(defconst wild_life-mode-smie-grammar
  ;; Rather than construct the operator levels table from the BNF,
  ;; we directly provide the operator precedences from GNU Wild_Life-Mode's
  ;; manual (7.14.10 op/3).  The only problem is that GNU Wild_Life-Mode's
  ;; manual uses precedence levels in the opposite sense (higher
  ;; numbers bind less tightly) than SMIE, so we use negative numbers.
  '(("." -10000 -10000)
    (":-" -1200 -1200)
    ("-->" -1200 -1200)
    (";" -1100 -1100)
    ("->" -1050 -1050)
    ("," -1000 -1000)
    ("\\+" -900 -900)
    ("=" -700 -700)
    ("\\=" -700 -700)
    ("=.." -700 -700)
    ("==" -700 -700)
    ("\\==" -700 -700)
    ("@<" -700 -700)
    ("@=<" -700 -700)
    ("@>" -700 -700)
    ("@>=" -700 -700)
    ("is" -700 -700)
    ("=:=" -700 -700)
    ("=\\=" -700 -700)
    ("<" -700 -700)
    ("=<" -700 -700)
    (">" -700 -700)
    (">=" -700 -700)
    (":" -600 -600)
    ("+" -500 -500)
    ("-" -500 -500)
    ("/\\" -500 -500)
    ("\\/" -500 -500)
    ("*" -400 -400)
    ("/" -400 -400)
    ("//" -400 -400)
    ("rem" -400 -400)
    ("mod" -400 -400)
    ("<<" -400 -400)
    (">>" -400 -400)
    ("**" -200 -200)
    ("^" -200 -200)
    ;; Prefix
    ;; ("+" 200 200)
    ;; ("-" 200 200)
    ;; ("\\" 200 200)
    (:smie-closer-alist (t . "."))
    )
  "Precedence levels of infix operators.")

(defun wild_life-mode-smie-rules (kind token)
  (pcase (cons kind token)
    (`(:elem . basic) wild_life-mode-indent-width)
    (`(:after . ".") '(column . 0)) ;; To work around smie-closer-alist.
    ;; Allow indentation of if-then-else as:
    ;;    (   test
    ;;     -> thenrule
    ;;     ;  elserule
    ;;    )
    (`(:before . ,(or `"->" `";"))
     (and (smie-rule-bolp) (smie-rule-parent-p "(") (smie-rule-parent 1)))
    (`(:after . ,(or `":-" `"->" `"-->")) wild_life-mode-indent-width)))


;;-------------------------------------------------------------------
;; Wild_Life-Mode mode
;;-------------------------------------------------------------------

;; Example: (wild_life-mode-atleast-version '(3 . 6))
(defun wild_life-mode-atleast-version (version)
  "Return t if the version of the current wild_life-mode system is VERSION or later.
VERSION is of the format (Major . Minor)"
  ;; Version.major < major or
  ;; Version.major = major and Version.minor <= minor
  (let* ((thisversion (wild_life-mode-find-value-by-system wild_life-mode-system-version))
         (thismajor (car thisversion))
         (thisminor (cdr thisversion)))
    (or (< (car version) thismajor)
        (and (= (car version) thismajor)
             (<= (cdr version) thisminor)))
    ))

(define-abbrev-table 'wild_life-mode-abbrev-table ())

(defun wild_life-mode-find-value-by-system (alist)
  "Get value from ALIST according to `wild_life-mode-system'."
  (let ((system (or wild_life-mode-system
                    (let ((infbuf (wild_life-mode-inferior-buffer 'dont-run)))
                      (when infbuf
                        (buffer-local-value 'wild_life-mode-system infbuf))))))
    (if (listp alist)
        (let (result
              id)
          (while alist
            (setq id (car (car alist)))
            (if (or (eq id system)
                    (eq id t)
                    (and (listp id)
                         (eval id)))
                (progn
                  (setq result (car (cdr (car alist))))
                  (if (and (listp result)
                           (eq (car result) 'eval))
                      (setq result (eval (car (cdr result)))))
                  (setq alist nil))
              (setq alist (cdr alist))))
          result)
      alist)))

(defconst wild_life-mode-syntax-propertize-function
  (when (fboundp 'syntax-propertize-rules)
    (syntax-propertize-rules
     ;; GNU Wild_Life-Mode only accepts 0'\' rather than 0'', but the only
     ;; possible meaning of 0'' is rather clear.
     ("\\<0\\(''?\\)"
      (1 (unless (save-excursion (nth 8 (syntax-ppss (match-beginning 0))))
           (string-to-syntax "_"))))
     ;; We could check that we're not inside an atom, but I don't think
     ;; that 'foo 8'z could be a valid syntax anyway, so why bother?
     ("\\<[1-9][0-9]*\\('\\)[0-9a-zA-Z]" (1 "_"))
     ;; Supposedly, ISO-Wild_Life-Mode wants \NNN\ for octal and \xNNN\ for hexadecimal
     ;; escape sequences in atoms, so be careful not to let the terminating \
     ;; escape a subsequent quote.
     ("\\\\[x0-7][0-9a-fA-F]*\\(\\\\\\)" (1 "_"))
     )))

(defun wild_life-mode-variables ()
  "Set some common variables to Wild_Life-Mode code specific values."
  (setq-local local-abbrev-table wild_life-mode-abbrev-table)
  (setq-local paragraph-start (concat "[ \t]*$\\|" page-delimiter)) ;'%%..'
  (setq-local paragraph-separate paragraph-start)
  (setq-local paragraph-ignore-fill-prefix t)
  (setq-local normal-auto-fill-function 'wild_life-mode-do-auto-fill)
  (setq-local comment-start "%")
  (setq-local comment-end "")
  (setq-local comment-add 1)
  (setq-local comment-start-skip "\\(?:/\\*+ *\\|%%+ *\\)")
  (setq-local parens-require-spaces nil)
  ;; Initialize Wild_Life-Mode system specific variables
  (dolist (var '(wild_life-mode-keywords wild_life-mode-types wild_life-mode-specificators
                 wild_life-mode-determinism-specificators wild_life-mode-directives
                 wild_life-mode-eof-string
                 ;; wild_life-mode-continued-prompt-regexp
                 wild_life-mode-help-function))
    (set (intern (concat (symbol-name var) "-i"))
         (wild_life-mode-find-value-by-system (symbol-value var))))
  (when (null (wild_life-mode-program-name))
    (setq-local compile-command (wild_life-mode-compile-string)))
  (setq-local font-lock-defaults
              '(wild_life-mode-font-lock-keywords nil nil ((?_ . "w"))))
  (setq-local syntax-propertize-function wild_life-mode-syntax-propertize-function)

  (smie-setup wild_life-mode-smie-grammar #'wild_life-mode-smie-rules
              :forward-token #'wild_life-mode-smie-forward-token
              :backward-token #'wild_life-mode-smie-backward-token))

(defun wild_life-mode-keybindings-common (map)
  "Define keybindings common to both Wild_Life-Mode modes in MAP."
  (define-key map "\C-c?" 'wild_life-mode-help-on-predicate)
  (define-key map "\C-c/" 'wild_life-mode-help-apropos)
  (define-key map "\C-c\C-d" 'wild_life-mode-debug-on)
  (define-key map "\C-c\C-t" 'wild_life-mode-trace-on)
  (define-key map "\C-c\C-z" 'wild_life-mode-zip-on)
  (define-key map "\C-c\r" 'run-wild_life-mode))

(defun wild_life-mode-keybindings-edit (map)
  "Define keybindings for Wild_Life-Mode mode in MAP."
  (define-key map "\M-a" 'wild_life-mode-beginning-of-clause)
  (define-key map "\M-e" 'wild_life-mode-end-of-clause)
  (define-key map "\M-q" 'wild_life-mode-fill-paragraph)
  (define-key map "\C-c\C-a" 'align)
  (define-key map "\C-\M-a" 'wild_life-mode-beginning-of-predicate)
  (define-key map "\C-\M-e" 'wild_life-mode-end-of-predicate)
  (define-key map "\M-\C-c" 'wild_life-mode-mark-clause)
  (define-key map "\M-\C-h" 'wild_life-mode-mark-predicate)
  (define-key map "\C-c\C-n" 'wild_life-mode-insert-predicate-template)
  (define-key map "\C-c\C-s" 'wild_life-mode-insert-predspec)
  (define-key map "\M-\r" 'wild_life-mode-insert-next-clause)
  (define-key map "\C-c\C-va" 'wild_life-mode-variables-to-anonymous)
  (define-key map "\C-c\C-v\C-s" 'wild_life-mode-view-predspec)

  ;; If we're running SICStus, then map C-c C-c e/d to enabling
  ;; and disabling of the source-level debugging facilities.
  ;(if (and (eq wild_life-mode-system 'sicstus)
  ;         (wild_life-mode-atleast-version '(3 . 7)))
  ;    (progn
  ;      (define-key map "\C-c\C-ce" 'wild_life-mode-enable-sicstus-sd)
  ;      (define-key map "\C-c\C-cd" 'wild_life-mode-disable-sicstus-sd)
  ;      ))

  (if wild_life-mode-old-sicstus-keys-flag
      (progn
        (define-key map "\C-c\C-c" 'wild_life-mode-consult-predicate)
        (define-key map "\C-cc" 'wild_life-mode-consult-region)
        (define-key map "\C-cC" 'wild_life-mode-consult-buffer)
        (define-key map "\C-c\C-k" 'wild_life-mode-compile-predicate)
        (define-key map "\C-ck" 'wild_life-mode-compile-region)
        (define-key map "\C-cK" 'wild_life-mode-compile-buffer))
    (define-key map "\C-c\C-p" 'wild_life-mode-consult-predicate)
    (define-key map "\C-c\C-r" 'wild_life-mode-consult-region)
    (define-key map "\C-c\C-b" 'wild_life-mode-consult-buffer)
    (define-key map "\C-c\C-f" 'wild_life-mode-consult-file)
    (define-key map "\C-c\C-cp" 'wild_life-mode-compile-predicate)
    (define-key map "\C-c\C-cr" 'wild_life-mode-compile-region)
    (define-key map "\C-c\C-cb" 'wild_life-mode-compile-buffer)
    (define-key map "\C-c\C-cf" 'wild_life-mode-compile-file))

  ;; Inherited from the old wild_life-mode.el.
  (define-key map "\e\C-x" 'wild_life-mode-consult-region)
  (define-key map "\C-c\C-l" 'wild_life-mode-consult-file)
  (define-key map "\C-c\C-z" 'run-wild_life-mode))

(defun wild_life-mode-keybindings-inferior (_map)
  "Define keybindings for inferior Wild_Life-Mode mode in MAP."
  ;; No inferior mode specific keybindings now.
  )

(defvar wild_life-mode-map
  (let ((map (make-sparse-keymap)))
    (wild_life-mode-keybindings-common map)
    (wild_life-mode-keybindings-edit map)
    map))


(defvar wild_life-mode-hook nil
  "List of functions to call after the wild_life-mode mode has initialized.")

;;;###autoload
(define-derived-mode wild_life-mode prog-mode "Wild_Life-Mode"
  "Major mode for editing Wild_Life-Mode code.

Blank lines and `%%...' separate paragraphs.  `%'s starts a comment
line and comments can also be enclosed in /* ... */.

If an optional argument SYSTEM is non-nil, set up mode for the given system.

To find out what version of Wild_Life-Mode mode you are running, enter
`\\[wild_life-mode-version]'.

Commands:
\\{wild_life-mode-map}"
  (setq mode-name (concat "Wild_Life-Mode"
                          (cond
                           ((eq wild_life-mode-system 'eclipse) "[ECLiPSe]")
                           ((eq wild_life-mode-system 'sicstus) "[SICStus]")
                           ((eq wild_life-mode-system 'swi) "[SWI]")
                           ((eq wild_life-mode-system 'gnu) "[GNU]")
                           (t ""))))
  (wild_life-mode-variables)
  (dolist (ar wild_life-mode-align-rules) (add-to-list 'align-rules-list ar))
  (add-hook 'post-self-insert-hook #'wild_life-mode-post-self-insert nil t)
  ;; `imenu' entry moved to the appropriate hook for consistency.

  ;; Load SICStus debugger if suitable
  (if (and (eq wild_life-mode-system 'sicstus)
           (wild_life-mode-atleast-version '(3 . 7))
           wild_life-mode-use-sicstus-sd)
      (wild_life-mode-enable-sicstus-sd))

  (wild_life-mode-menu))

(defvar mercury-mode-map
  (let ((map (make-sparse-keymap)))
    (set-keymap-parent map wild_life-mode-map)
    map))

;;;###autoload
(define-derived-mode mercury-mode wild_life-mode "Wild_Life-Mode[Mercury]"
  "Major mode for editing Mercury programs.
Actually this is just customized `wild_life-mode'."
  (setq-local wild_life-mode-system 'mercury))


;;-------------------------------------------------------------------
;; Inferior wild_life-mode mode
;;-------------------------------------------------------------------

(defvar wild_life-mode-inferior-mode-map
  (let ((map (make-sparse-keymap)))
    (wild_life-mode-keybindings-common map)
    (wild_life-mode-keybindings-inferior map)
    (define-key map [remap self-insert-command]
      'wild_life-mode-inferior-self-insert-command)
    map))

(defvar wild_life-mode-inferior-mode-hook nil
  "List of functions to call after the inferior wild_life-mode mode has initialized.")

(defvar wild_life-mode-inferior-error-regexp-alist
  '(;; GNU Wild_Life-Mode used to not follow the GNU standard format.
    ;; ("^\\(.*?\\):\\([0-9]+\\) error: .*(char:\\([0-9]+\\)" 1 2 3)
    ;; Wild_Life-Mode.
    ("^\\(?:\\?- *\\)?\\(\\(?:ERROR\\|\\(W\\)arning\\): *\\(.*?\\):\\([1-9][0-9]*\\):\\(?:\\([0-9]*\\):\\)?\\)\\(?:$\\| \\)"
     3 4 5 (2 . nil) 1)
    ;; GNU-Wild_Life-Mode now uses the GNU standard format.
    gnu))

(defun wild_life-mode-inferior-self-insert-command ()
  "Insert the char in the buffer or pass it directly to the process."
  (interactive)
  (let* ((proc (get-buffer-process (current-buffer)))
         (pmark (and proc (marker-position (process-mark proc)))))
    ;; FIXME: the same treatment would be needed for Wild_Life-Mode, but I can't
    ;; seem to find any way for Emacs to figure out when to use it because
    ;; SWI doesn't include a " ? " or some such recognizable marker.
    (if (and (eq wild_life-mode-system 'gnu)
             pmark
             (null current-prefix-arg)
             (eobp)
             (eq (point) pmark)
             (save-excursion
               (goto-char (- pmark 3))
               ;; FIXME: check this comes from the process's output, maybe?
               (looking-at " \\? ")))
        ;; This is GNU wild_life-mode waiting to know whether you want more answers
        ;; or not (or abort, etc...).  The answer is a single char, not
        ;; a line, so pass this char directly rather than wait for RET to
        ;; send a whole line.
        (comint-send-string proc (string last-command-event))
      (call-interactively 'self-insert-command))))

(declare-function 'compilation-shell-minor-mode "compile" (&optional arg))
(defvar compilation-error-regexp-alist)

(define-derived-mode wild_life-mode-inferior-mode comint-mode "Inferior Wild_Life-Mode"
  "Major mode for interacting with an inferior Wild_Life-Mode process.

The following commands are available:
\\{wild_life-mode-inferior-mode-map}

Entry to this mode calls the value of `wild_life-mode-hook' with no arguments,
if that value is non-nil.  Likewise with the value of `comint-mode-hook'.
`wild_life-mode-hook' is called after `comint-mode-hook'.

You can send text to the inferior Wild_Life-Mode from other buffers
using the commands `send-region', `send-string' and \\[wild_life-mode-consult-region].

Commands:
Tab indents for Wild_Life-Mode; with argument, shifts rest
 of expression rigidly with the current line.
Paragraphs are separated only by blank lines and '%%'. '%'s start comments.

Return at end of buffer sends line as input.
Return not at end copies rest of line to end and sends it.
\\[comint-delchar-or-maybe-eof] sends end-of-file as input.
\\[comint-kill-input] and \\[backward-kill-word] are kill commands,
imitating normal Unix input editing.
\\[comint-interrupt-subjob] interrupts the shell or its current subjob if any.
\\[comint-stop-subjob] stops, likewise.
\\[comint-quit-subjob] sends quit signal, likewise.

To find out what version of Wild_Life-Mode mode you are running, enter
`\\[wild_life-mode-version]'."
  (require 'compile)
  (setq comint-input-filter 'wild_life-mode-input-filter)
  (setq mode-line-process '(": %s"))
  (wild_life-mode-variables)
  (setq comint-prompt-regexp (wild_life-mode-prompt-regexp))
  (setq-local shell-dirstack-query "pwd.")
  (setq-local compilation-error-regexp-alist
              wild_life-mode-inferior-error-regexp-alist)
  (compilation-shell-minor-mode)
  (wild_life-mode-inferior-menu))

(defun wild_life-mode-input-filter (str)
  (cond ((string-match "\\`\\s *\\'" str) nil) ;whitespace
        ((not (derived-mode-p 'wild_life-mode-inferior-mode)) t)
        ((= (length str) 1) nil)        ;one character
        ((string-match "\\`[rf] *[0-9]*\\'" str) nil) ;r(edo) or f(ail)
        (t t)))

;; This statement was missing in Emacs 24.1, 24.2, 24.3.
(define-obsolete-function-alias 'switch-to-wild_life-mode 'run-wild_life-mode "24.1")
;;;###autoload
(defun run-wild_life-mode (arg)
  "Run an inferior Wild_Life-Mode process, input and output via buffer *wild_life-mode*.
With prefix argument ARG, restart the Wild_Life-Mode process if running before."
  (interactive "P")
  ;; FIXME: It should be possible to interactively specify the command to use
  ;; to run wild_life-mode.
  (if (and arg (get-process "wild_life-mode"))
      (progn
        (process-send-string "wild_life-mode" "halt.\n")
        (while (get-process "wild_life-mode") (sit-for 0.1))))
  (let ((buff (buffer-name)))
    (if (not (string= buff "*wild_life-mode*"))
        (wild_life-mode-goto-wild_life-mode-process-buffer))
    ;; Load SICStus debugger if suitable
    (if (and (eq wild_life-mode-system 'sicstus)
             (wild_life-mode-atleast-version '(3 . 7))
             wild_life-mode-use-sicstus-sd)
        (wild_life-mode-enable-sicstus-sd))
    (wild_life-mode-variables)
    (wild_life-mode-ensure-process)
    ))

(defun wild_life-mode-inferior-guess-flavor (&optional ignored)
  (setq-local wild_life-mode-system
              (when (or (numberp wild_life-mode-system) (markerp wild_life-mode-system))
                (save-excursion
                  (goto-char (1+ wild_life-mode-system))
                  (cond
                   ((looking-at "GNU Wild_Life-Mode") 'gnu)
                   ((looking-at "Welcome to Wild_Life-Mode\\|%.*\\<swi_") 'swi)
                   ((looking-at ".*\n") nil) ;There's at least one line.
                   (t wild_life-mode-system)))))
  (when (symbolp wild_life-mode-system)
    (remove-hook 'comint-output-filter-functions
                 'wild_life-mode-inferior-guess-flavor t)
    (when wild_life-mode-system
      (setq comint-prompt-regexp (wild_life-mode-prompt-regexp))
      (if (eq wild_life-mode-system 'gnu)
          (setq-local comint-process-echoes t)))))

(defun wild_life-mode-ensure-process (&optional wait)
  "If Wild_Life-Mode process is not running, run it.
If the optional argument WAIT is non-nil, wait for Wild_Life-Mode prompt specified by
the variable `wild_life-mode-prompt-regexp'."
  (if (null (wild_life-mode-program-name))
      (error "This Wild_Life-Mode system has defined no interpreter."))
  (if (comint-check-proc "*wild_life-mode*")
      ()
    (with-current-buffer (get-buffer-create "*wild_life-mode*")
      (wild_life-mode-inferior-mode)
      (apply 'make-comint-in-buffer "wild_life-mode" (current-buffer)
             (wild_life-mode-program-name) nil (wild_life-mode-program-switches))
      (unless wild_life-mode-system
        ;; Setup auto-detection.
        (setq-local
         wild_life-mode-system
         ;; Force re-detection.
         (let* ((proc (get-buffer-process (current-buffer)))
                (pmark (and proc (marker-position (process-mark proc)))))
           (cond
            ((null pmark) (1- (point-min)))
            ;; The use of insert-before-markers in comint.el together with
            ;; the potential use of comint-truncate-buffer in the output
            ;; filter, means that it's difficult to reliably keep track of
            ;; the buffer position where the process's output started.
            ;; If possible we use a marker at "start - 1", so that
            ;; insert-before-marker at `start' won't shift it.  And if not,
            ;; we fall back on using a plain integer.
            ((> pmark (point-min)) (copy-marker (1- pmark)))
            (t (1- pmark)))))
        (add-hook 'comint-output-filter-functions
                  'wild_life-mode-inferior-guess-flavor nil t))
      (if wait
          (progn
            (goto-char (point-max))
            (while
                (save-excursion
                  (not
                   (re-search-backward
                    (concat "\\(" (wild_life-mode-prompt-regexp) "\\)" "\\=")
                    nil t)))
              (sit-for 0.1)))))))

(defun wild_life-mode-inferior-buffer (&optional dont-run)
  (or (get-buffer "*wild_life-mode*")
      (unless dont-run
        (wild_life-mode-ensure-process)
        (get-buffer "*wild_life-mode*"))))

(defun wild_life-mode-process-insert-string (process string)
  "Insert STRING into inferior Wild_Life-Mode buffer running PROCESS."
  ;; Copied from elisp manual, greek to me
  (with-current-buffer (process-buffer process)
    ;; FIXME: Use window-point-insertion-type instead.
    (let ((moving (= (point) (process-mark process))))
      (save-excursion
        ;; Insert the text, moving the process-marker.
        (goto-char (process-mark process))
        (insert string)
        (set-marker (process-mark process) (point)))
      (if moving (goto-char (process-mark process))))))

;;------------------------------------------------------------
;; Old consulting and compiling functions
;;------------------------------------------------------------

(declare-function compilation-forget-errors "compile" ())
(declare-function compilation-fake-loc "compile"
                  (marker file &optional line col))

(defun wild_life-mode-old-process-region (compilep start end)
  "Process the region limited by START and END positions.
If COMPILEP is non-nil then use compilation, otherwise consulting."
   (wild_life-mode-ensure-process)
   ;(let ((tmpfile wild_life-mode-temp-filename)
   (let ((tmpfile (wild_life-mode-temporary-file))
         ;(process (get-process "wild_life-mode"))
         (first-line (1+ (count-lines
                          (point-min)
                          (save-excursion
                            (goto-char start)
                            (point))))))
     (write-region start end tmpfile)
     (setq start (copy-marker start))
     (with-current-buffer (wild_life-mode-inferior-buffer)
       (compilation-forget-errors)
       (compilation-fake-loc start tmpfile))
     (process-send-string
      "wild_life-mode" (wild_life-mode-build-wild_life-mode-command
                compilep tmpfile (wild_life-mode-bsts buffer-file-name)
                first-line))
     (wild_life-mode-goto-wild_life-mode-process-buffer)))

(defun wild_life-mode-old-process-predicate (compilep)
  "Process the predicate around point.
If COMPILEP is non-nil then use compilation, otherwise consulting."
  (wild_life-mode-old-process-region
   compilep (wild_life-mode-pred-start) (wild_life-mode-pred-end)))

(defun wild_life-mode-old-process-buffer (compilep)
  "Process the entire buffer.
If COMPILEP is non-nil then use compilation, otherwise consulting."
  (wild_life-mode-old-process-region compilep (point-min) (point-max)))

(defun wild_life-mode-old-process-file (compilep)
  "Process the file of the current buffer.
If COMPILEP is non-nil then use compilation, otherwise consulting."
  (save-some-buffers)
  (wild_life-mode-ensure-process)
  (with-current-buffer (wild_life-mode-inferior-buffer)
    (compilation-forget-errors))
    (process-send-string
     "wild_life-mode" (wild_life-mode-build-wild_life-mode-command
             compilep buffer-file-name
             (wild_life-mode-bsts buffer-file-name)))
  (wild_life-mode-goto-wild_life-mode-process-buffer))


;;------------------------------------------------------------
;; Consulting and compiling
;;------------------------------------------------------------

;; Interactive interface functions, used by both the standard
;; and the experimental consultation and compilation functions
(defun wild_life-mode-consult-file ()
  "Consult file of current buffer."
  (interactive)
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-file nil)
    (wild_life-mode-consult-compile-file nil)))

(defun wild_life-mode-consult-buffer ()
  "Consult buffer."
  (interactive)
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-buffer nil)
    (wild_life-mode-consult-compile-buffer nil)))

(defun wild_life-mode-consult-region (beg end)
  "Consult region between BEG and END."
  (interactive "r")
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-region nil beg end)
    (wild_life-mode-consult-compile-region nil beg end)))

(defun wild_life-mode-consult-predicate ()
  "Consult the predicate around current point."
  (interactive)
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-predicate nil)
    (wild_life-mode-consult-compile-predicate nil)))

(defun wild_life-mode-compile-file ()
  "Compile file of current buffer."
  (interactive)
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-file t)
    (wild_life-mode-consult-compile-file t)))

(defun wild_life-mode-compile-buffer ()
  "Compile buffer."
  (interactive)
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-buffer t)
    (wild_life-mode-consult-compile-buffer t)))

(defun wild_life-mode-compile-region (beg end)
  "Compile region between BEG and END."
  (interactive "r")
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-region t beg end)
    (wild_life-mode-consult-compile-region t beg end)))

(defun wild_life-mode-compile-predicate ()
  "Compile the predicate around current point."
  (interactive)
  (if wild_life-mode-use-standard-consult-compile-method-flag
      (wild_life-mode-old-process-predicate t)
    (wild_life-mode-consult-compile-predicate t)))

(defun wild_life-mode-buffer-module ()
  "Select Wild_Life-Mode module name appropriate for current buffer.
Bases decision on buffer contents (-*- line)."
  ;; Look for -*- ... module: MODULENAME; ... -*-
  (let (beg end)
    (save-excursion
      (goto-char (point-min))
      (skip-chars-forward " \t")
      (and (search-forward "-*-" (line-end-position) t)
           (progn
             (skip-chars-forward " \t")
             (setq beg (point))
             (search-forward "-*-" (line-end-position) t))
           (progn
             (forward-char -3)
             (skip-chars-backward " \t")
             (setq end (point))
             (goto-char beg)
             (and (let ((case-fold-search t))
                    (search-forward "module:" end t))
                  (progn
                    (skip-chars-forward " \t")
                    (setq beg (point))
                    (if (search-forward ";" end t)
                        (forward-char -1)
                      (goto-char end))
                    (skip-chars-backward " \t")
                    (buffer-substring beg (point)))))))))

(defun wild_life-mode-build-wild_life-mode-command (compilep file buffername
                                    &optional first-line)
  "Make Wild_Life-Mode command for FILE compilation/consulting.
If COMPILEP is non-nil, consider compilation, otherwise consulting."
  (let* ((compile-string
          ;; FIXME: If the process is not running yet, the auto-detection of
          ;; wild_life-mode-system won't help here, so we should make sure
          ;; we first run Wild_Life-Mode and then build the command.
          (if compilep (wild_life-mode-compile-string) (wild_life-mode-consult-string)))
         (module (wild_life-mode-buffer-module))
         (file-name (concat "'" (wild_life-mode-bsts file) "'"))
         (module-name (if module (concat "'" module "'")))
         (module-file (if module
                          (concat module-name ":" file-name)
                        file-name))
         strbeg strend
         (lineoffset (if first-line
                         (- first-line 1)
                       0)))

    ;; Assure that there is a buffer name
    (if (not buffername)
        (error "The buffer is not saved"))

    (if (not (string-match "\\`'.*'\\'" buffername)) ; Add quotes
        (setq buffername (concat "'" buffername "'")))
    (while (string-match "%m" compile-string)
      (setq strbeg (substring compile-string 0 (match-beginning 0)))
      (setq strend (substring compile-string (match-end 0)))
      (setq compile-string (concat strbeg module-file strend)))
    ;; FIXME: The code below will %-expand any %[fbl] that appears in
    ;; module-file.
    (while (string-match "%f" compile-string)
      (setq strbeg (substring compile-string 0 (match-beginning 0)))
      (setq strend (substring compile-string (match-end 0)))
      (setq compile-string (concat strbeg file-name strend)))
    (while (string-match "%b" compile-string)
      (setq strbeg (substring compile-string 0 (match-beginning 0)))
      (setq strend (substring compile-string (match-end 0)))
      (setq compile-string (concat strbeg buffername strend)))
    (while (string-match "%l" compile-string)
      (setq strbeg (substring compile-string 0 (match-beginning 0)))
      (setq strend (substring compile-string (match-end 0)))
      (setq compile-string (concat strbeg (format "%d" lineoffset) strend)))
    (concat compile-string "\n")))

;; The rest of this page is experimental code!

;; Global variables for process filter function
(defvar wild_life-mode-process-flag nil
  "Non-nil means that a wild_life-mode task (i.e. a consultation or compilation job)
is running.")
(defvar wild_life-mode-consult-compile-output ""
  "Hold the unprocessed output from the current wild_life-mode task.")
(defvar wild_life-mode-consult-compile-first-line 1
  "The number of the first line of the file to consult/compile.
Used for temporary files.")
(defvar wild_life-mode-consult-compile-file nil
  "The file to compile/consult (can be a temporary file).")
(defvar wild_life-mode-consult-compile-real-file nil
  "The file name of the buffer to compile/consult.")

(defvar compilation-parse-errors-function)

(defun wild_life-mode-consult-compile (compilep file &optional first-line)
  "Consult/compile FILE.
If COMPILEP is non-nil, perform compilation, otherwise perform CONSULTING.
COMMAND is a string described by the variables `wild_life-mode-consult-string'
and `wild_life-mode-compile-string'.
Optional argument FIRST-LINE is the number of the first line in the compiled
region.

This function must be called from the source code buffer."
  (if wild_life-mode-process-flag
      (error "Another Wild_Life-Mode task is running."))
  (wild_life-mode-ensure-process t)
  (let* ((buffer (get-buffer-create wild_life-mode-compilation-buffer))
         (real-file buffer-file-name)
         (command-string (wild_life-mode-build-wild_life-mode-command compilep file
                                                      real-file first-line))
         (process (get-process "wild_life-mode")))
    (with-current-buffer buffer
      (delete-region (point-min) (point-max))
      ;; FIXME: Wasn't this supposed to use wild_life-mode-inferior-mode?
      (compilation-mode)
      ;; FIXME: This doesn't seem to cooperate well with new(ish) compile.el.
      ;; Setting up font-locking for this buffer
      (setq-local font-lock-defaults
                  '(wild_life-mode-font-lock-keywords nil nil ((?_ . "w"))))
      (if (eq wild_life-mode-system 'sicstus)
          ;; FIXME: This looks really problematic: not only is this using
          ;; the old compilation-parse-errors-function, but
          ;; wild_life-mode-parse-sicstus-compilation-errors only accepts one argument
          ;; whereas compile.el calls it with 2 (and did so at least since
          ;; Emacs-20).
          (setq-local compilation-parse-errors-function
                      'wild_life-mode-parse-sicstus-compilation-errors))
      (setq buffer-read-only nil)
      (insert command-string "\n"))
    (display-buffer buffer)
    (setq wild_life-mode-process-flag t
          wild_life-mode-consult-compile-output ""
          wild_life-mode-consult-compile-first-line (if first-line (1- first-line) 0)
          wild_life-mode-consult-compile-file file
          wild_life-mode-consult-compile-real-file (if (string=
                                                file buffer-file-name)
                                               nil
                                             real-file))
    (with-current-buffer buffer
      (goto-char (point-max))
      (add-function :override (process-filter process)
                    #'wild_life-mode-consult-compile-filter)
      (process-send-string "wild_life-mode" command-string)
      ;; (wild_life-mode-build-wild_life-mode-command compilep file real-file first-line))
      (while (and wild_life-mode-process-flag
                  (accept-process-output process 10)) ; 10 secs is ok?
        (sit-for 0.1)
        (unless (get-process "wild_life-mode")
          (setq wild_life-mode-process-flag nil)))
      (insert (if compilep
                  "\nCompilation finished.\n"
                "\nConsulted.\n"))
      (remove-function (process-filter process)
                       #'wild_life-mode-consult-compile-filter))))

(defvar compilation-error-list)

(defun wild_life-mode-parse-sicstus-compilation-errors (limit)
  "Parse the wild_life-mode compilation buffer for errors.
Argument LIMIT is a buffer position limiting searching.
For use with the `compilation-parse-errors-function' variable."
  (setq compilation-error-list nil)
  (message "Parsing SICStus error messages...")
  (let (filepath dir file errorline)
    (while
        (re-search-backward
         "{\\([a-zA-Z ]* ERROR\\|Warning\\):.* in line[s ]*\\([0-9]+\\)"
         limit t)
      (setq errorline (string-to-number (match-string 2)))
      (save-excursion
        (re-search-backward
         "{\\(consulting\\|compiling\\|processing\\) \\(.*\\)\\.\\.\\.}"
         limit t)
        (setq filepath (match-string 2)))

      ;; ###### Does this work with SICStus under Windows
      ;; (i.e. backslashes and stuff?)
      (if (string-match "\\(.*/\\)\\([^/]*\\)$" filepath)
          (progn
            (setq dir (match-string 1 filepath))
            (setq file (match-string 2 filepath))))

      (setq compilation-error-list
            (cons
             (cons (save-excursion
                     (beginning-of-line)
                     (point-marker))
                   (list (list file dir) errorline))
             compilation-error-list)
            ))
    ))

(defun wild_life-mode-consult-compile-filter (process output)
  "Filter function for Wild_Life-Mode compilation PROCESS.
Argument OUTPUT is a name of the output file."
  ;;(message "start")
  (setq wild_life-mode-consult-compile-output
        (concat wild_life-mode-consult-compile-output output))
  ;;(message "pccf1: %s" wild_life-mode-consult-compile-output)
  ;; Iterate through the lines of wild_life-mode-consult-compile-output
  (let (outputtype)
    (while (and wild_life-mode-process-flag
                (or
                 ;; Trace question
                 (progn
                   (setq outputtype 'trace)
                   (and (eq wild_life-mode-system 'sicstus)
                        (string-match
                         "^[ \t]*[0-9]+[ \t]*[0-9]+[ \t]*Call:.*? "
                         wild_life-mode-consult-compile-output)))

                 ;; Match anything
                 (progn
                   (setq outputtype 'normal)
                   (string-match "^.*\n" wild_life-mode-consult-compile-output))
                   ))
      ;;(message "outputtype: %s" outputtype)

      (setq output (match-string 0 wild_life-mode-consult-compile-output))
      ;; remove the text in output from wild_life-mode-consult-compile-output
      (setq wild_life-mode-consult-compile-output
            (substring wild_life-mode-consult-compile-output (length output)))
      ;;(message "pccf2: %s" wild_life-mode-consult-compile-output)

      ;; If temporary files were used, then we change the error
      ;; messages to point to the original source file.
      ;; FIXME: Use compilation-fake-loc instead.
      (cond

       ;; If the wild_life-mode process was in trace mode then it requires
       ;; user input
       ((and (eq wild_life-mode-system 'sicstus)
             (eq outputtype 'trace))
        (let ((input (concat (read-string output) "\n")))
          (process-send-string process input)
          (setq output (concat output input))))

       ((eq wild_life-mode-system 'sicstus)
        (if (and wild_life-mode-consult-compile-real-file
                 (string-match
                  "\\({.*:.* in line[s ]*\\)\\([0-9]+\\)-\\([0-9]+\\)" output))
            (setq output (replace-match
                          ;; Adds a {processing ...} line so that
                          ;; `wild_life-mode-parse-sicstus-compilation-errors'
                          ;; finds the real file instead of the temporary one.
                          ;; Also fixes the line numbers.
                          (format "Added by Emacs: {processing %s...}\n%s%d-%d"
                                  wild_life-mode-consult-compile-real-file
                                  (match-string 1 output)
                                  (+ wild_life-mode-consult-compile-first-line
                                     (string-to-number
                                      (match-string 2 output)))
                                  (+ wild_life-mode-consult-compile-first-line
                                     (string-to-number
                                      (match-string 3 output))))
                          t t output)))
        )

       ((eq wild_life-mode-system 'swi)
        (if (and wild_life-mode-consult-compile-real-file
                 (string-match (format
                                "%s\\([ \t]*:[ \t]*\\)\\([0-9]+\\)"
                                wild_life-mode-consult-compile-file)
                               output))
            (setq output (replace-match
                          ;; Real filename + text + fixed linenum
                          (format "%s%s%d"
                                  wild_life-mode-consult-compile-real-file
                                  (match-string 1 output)
                                  (+ wild_life-mode-consult-compile-first-line
                                     (string-to-number
                                      (match-string 2 output))))
                          t t output)))
        )

       (t ())
       )
      ;; Write the output in the *wild_life-mode-compilation* buffer
      (insert output)))

  ;; If the prompt is visible, then the task is finished
  (if (string-match (wild_life-mode-prompt-regexp) wild_life-mode-consult-compile-output)
      (setq wild_life-mode-process-flag nil)))

(defun wild_life-mode-consult-compile-file (compilep)
  "Consult/compile file of current buffer.
If COMPILEP is non-nil, compile, otherwise consult."
  (let ((file buffer-file-name))
    (if file
        (progn
          (save-some-buffers)
          (wild_life-mode-consult-compile compilep file))
      (wild_life-mode-consult-compile-region compilep (point-min) (point-max)))))

(defun wild_life-mode-consult-compile-buffer (compilep)
  "Consult/compile current buffer.
If COMPILEP is non-nil, compile, otherwise consult."
  (wild_life-mode-consult-compile-region compilep (point-min) (point-max)))

(defun wild_life-mode-consult-compile-region (compilep beg end)
  "Consult/compile region between BEG and END.
If COMPILEP is non-nil, compile, otherwise consult."
  ;(let ((file wild_life-mode-temp-filename)
  (let ((file (wild_life-mode-bsts (wild_life-mode-temporary-file)))
        (lines (count-lines 1 beg)))
    (write-region beg end file nil 'no-message)
    (write-region "\n" nil file t 'no-message)
    (wild_life-mode-consult-compile compilep file
                            (if (bolp) (1+ lines) lines))
    (delete-file file)))

(defun wild_life-mode-consult-compile-predicate (compilep)
  "Consult/compile the predicate around current point.
If COMPILEP is non-nil, compile, otherwise consult."
  (wild_life-mode-consult-compile-region
   compilep (wild_life-mode-pred-start) (wild_life-mode-pred-end)))


;;-------------------------------------------------------------------
;; Font-lock stuff
;;-------------------------------------------------------------------

;; Auxiliary functions

(defun wild_life-mode-font-lock-object-matcher (bound)
  "Find SICStus objects method name for font lock.
Argument BOUND is a buffer position limiting searching."
  (let (point
        (case-fold-search nil))
    (while (and (not point)
                (re-search-forward "\\(::[ \t\n]*{\\|&\\)[ \t]*"
                                   bound t))
      (while (or (re-search-forward "\\=\n[ \t]*" bound t)
                 (re-search-forward "\\=%.*" bound t)
                 (and (re-search-forward "\\=/\\*" bound t)
                      (re-search-forward "\\*/[ \t]*" bound t))))
      (setq point (re-search-forward
                   (format "\\=\\(%s\\)" wild_life-mode-atom-regexp)
                   bound t)))
    point))

(defsubst wild_life-mode-face-name-p (facename)
  ;; Return t if FACENAME is the name of a face.  This method is
  ;; necessary since facep in XEmacs only returns t for the actual
  ;; face objects (while it's only their names that are used just
  ;; about anywhere else) without providing a predicate that tests
  ;; face names.  This function (including the above commentary) is
  ;; borrowed from cc-mode.
  (memq facename (face-list)))

;; Set everything up
(defun wild_life-mode-font-lock-keywords ()
  "Set up font lock keywords for the current Wild_Life-Mode system."
  ;;(when window-system
  (require 'font-lock)

  ;; Define Wild_Life-Mode faces
  (defface wild_life-mode-redo-face
    '((((class grayscale)) (:italic t))
      (((class color)) (:foreground "darkorchid"))
      (t (:italic t)))
    "Wild_Life-Mode mode face for highlighting redo trace lines."
    :group 'wild_life-mode-faces)
  (defface wild_life-mode-exit-face
    '((((class grayscale)) (:underline t))
      (((class color) (background dark)) (:foreground "green"))
      (((class color) (background light)) (:foreground "ForestGreen"))
      (t (:underline t)))
    "Wild_Life-Mode mode face for highlighting exit trace lines."
    :group 'wild_life-mode-faces)
  (defface wild_life-mode-exception-face
    '((((class grayscale)) (:bold t :italic t :underline t))
      (((class color)) (:bold t :foreground "black" :background "Khaki"))
      (t (:bold t :italic t :underline t)))
    "Wild_Life-Mode mode face for highlighting exception trace lines."
    :group 'wild_life-mode-faces)
  (defface wild_life-mode-warning-face
    '((((class grayscale)) (:underline t))
      (((class color) (background dark)) (:foreground "blue"))
      (((class color) (background light)) (:foreground "MidnightBlue"))
      (t (:underline t)))
    "Face name to use for compiler warnings."
    :group 'wild_life-mode-faces)
  (defface wild_life-mode-builtin-face
    '((((class color) (background light)) (:foreground "Purple"))
      (((class color) (background dark)) (:foreground "Cyan"))
      (((class grayscale) (background light))
       :foreground "LightGray" :bold t)
      (((class grayscale) (background dark)) (:foreground "DimGray" :bold t))
      (t (:bold t)))
    "Face name to use for compiler warnings."
    :group 'wild_life-mode-faces)
  (defvar wild_life-mode-warning-face
    (if (wild_life-mode-face-name-p 'font-lock-warning-face)
        'font-lock-warning-face
      'wild_life-mode-warning-face)
    "Face name to use for built in predicates.")
  (defvar wild_life-mode-builtin-face
    (if (wild_life-mode-face-name-p 'font-lock-builtin-face)
        'font-lock-builtin-face
      'wild_life-mode-builtin-face)
    "Face name to use for built in predicates.")
  (defvar wild_life-mode-redo-face 'wild_life-mode-redo-face
    "Face name to use for redo trace lines.")
  (defvar wild_life-mode-exit-face 'wild_life-mode-exit-face
    "Face name to use for exit trace lines.")
  (defvar wild_life-mode-exception-face 'wild_life-mode-exception-face
    "Face name to use for exception trace lines.")

  ;; Font Lock Patterns
  (let (
        ;; "Native" Wild_Life-Mode patterns
        (head-predicates
         (list (format "^\\(%s\\)\\((\\|[ \t]*:-\\)" wild_life-mode-atom-regexp)
               1 font-lock-function-name-face))
                                       ;(list (format "^%s" wild_life-mode-atom-regexp)
                                       ;      0 font-lock-function-name-face))
        (head-predicates-1
         (list (format "\\.[ \t]*\\(%s\\)" wild_life-mode-atom-regexp)
               1 font-lock-function-name-face) )
        (variables
         '("\\<\\([_A-Z][a-zA-Z0-9_]*\\)"
           1 font-lock-variable-name-face))
        (important-elements
         (list (if (eq wild_life-mode-system 'mercury)
                   "[][}{;|]\\|\\\\[+=]\\|<?=>?"
                 "[][}{!;|]\\|\\*->")
               0 'font-lock-keyword-face))
        (important-elements-1
         '("[^-*]\\(->\\)" 1 font-lock-keyword-face))
        (predspecs                      ; module:predicate/cardinality
         (list (format "\\<\\(%s:\\|\\)%s/[0-9]+"
                       wild_life-mode-atom-regexp wild_life-mode-atom-regexp)
               0 font-lock-function-name-face 'prepend))
        (keywords                       ; directives (queries)
         (list
          (if (eq wild_life-mode-system 'mercury)
              (concat
               "\\<\\("
               (regexp-opt wild_life-mode-keywords-i)
               "\\|"
               (regexp-opt
                wild_life-mode-determinism-specificators-i)
               "\\)\\>")
            (concat
             "^[?:]- *\\("
             (regexp-opt wild_life-mode-keywords-i)
             "\\)\\>"))
          1 wild_life-mode-builtin-face))
        ;; SICStus specific patterns
        (sicstus-object-methods
         (if (eq wild_life-mode-system 'sicstus)
             '(wild_life-mode-font-lock-object-matcher
               1 font-lock-function-name-face)))
        ;; Mercury specific patterns
        (types
         (if (eq wild_life-mode-system 'mercury)
             (list
              (regexp-opt wild_life-mode-types-i 'words)
              0 'font-lock-type-face)))
        (modes
         (if (eq wild_life-mode-system 'mercury)
             (list
              (regexp-opt wild_life-mode-specificators-i 'words)
              0 'font-lock-constant-face)))
        (directives
         (if (eq wild_life-mode-system 'mercury)
             (list
              (regexp-opt wild_life-mode-directives-i 'words)
              0 'wild_life-mode-warning-face)))
        ;; Inferior mode specific patterns
        (prompt
         ;; FIXME: Should be handled by comint already.
         (list (wild_life-mode-prompt-regexp) 0 'font-lock-keyword-face))
        (trace-exit
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("[ \t]*[0-9]+[ \t]+[0-9]+[ \t]*\\(Exit\\):"
             1 wild_life-mode-exit-face))
          ((eq wild_life-mode-system 'swi)
           '("[ \t]*\\(Exit\\):[ \t]*([ \t0-9]*)" 1 wild_life-mode-exit-face))
          (t nil)))
        (trace-fail
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("[ \t]*[0-9]+[ \t]+[0-9]+[ \t]*\\(Fail\\):"
             1 wild_life-mode-warning-face))
          ((eq wild_life-mode-system 'swi)
           '("[ \t]*\\(Fail\\):[ \t]*([ \t0-9]*)" 1 wild_life-mode-warning-face))
          (t nil)))
        (trace-redo
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("[ \t]*[0-9]+[ \t]+[0-9]+[ \t]*\\(Redo\\):"
             1 wild_life-mode-redo-face))
          ((eq wild_life-mode-system 'swi)
           '("[ \t]*\\(Redo\\):[ \t]*([ \t0-9]*)" 1 wild_life-mode-redo-face))
          (t nil)))
        (trace-call
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("[ \t]*[0-9]+[ \t]+[0-9]+[ \t]*\\(Call\\):"
             1 font-lock-function-name-face))
          ((eq wild_life-mode-system 'swi)
           '("[ \t]*\\(Call\\):[ \t]*([ \t0-9]*)"
             1 font-lock-function-name-face))
          (t nil)))
        (trace-exception
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("[ \t]*[0-9]+[ \t]+[0-9]+[ \t]*\\(Exception\\):"
             1 wild_life-mode-exception-face))
          ((eq wild_life-mode-system 'swi)
           '("[ \t]*\\(Exception\\):[ \t]*([ \t0-9]*)"
             1 wild_life-mode-exception-face))
          (t nil)))
        (error-message-identifier
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("{\\([A-Z]* ?ERROR:\\)" 1 wild_life-mode-exception-face prepend))
          ((eq wild_life-mode-system 'swi)
           '("^[[]\\(WARNING:\\)" 1 wild_life-mode-builtin-face prepend))
          (t nil)))
        (error-whole-messages
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("{\\([A-Z]* ?ERROR:.*\\)}[ \t]*$"
             1 font-lock-comment-face append))
          ((eq wild_life-mode-system 'swi)
           '("^[[]WARNING:[^]]*[]]$" 0 font-lock-comment-face append))
          (t nil)))
        (error-warning-messages
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         ;; Mostly errors that SICStus asks the user about how to solve,
         ;; such as "NAME CLASH:" for example.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("^[A-Z ]*[A-Z]+:" 0 wild_life-mode-warning-face))
          (t nil)))
        (warning-messages
         ;; FIXME: Add to compilation-error-regexp-alist instead.
         (cond
          ((eq wild_life-mode-system 'sicstus)
           '("\\({ ?\\(Warning\\|WARNING\\) ?:.*}\\)[ \t]*$"
             2 wild_life-mode-warning-face prepend))
          (t nil))))

    ;; Make font lock list
    (delq
     nil
     (cond
      ((eq major-mode 'wild_life-mode)
       (list
        head-predicates
        head-predicates-1
        variables
        important-elements
        important-elements-1
        predspecs
        keywords
        sicstus-object-methods
        types
        modes
        directives))
      ((eq major-mode 'wild_life-mode-inferior-mode)
       (list
        prompt
        error-message-identifier
        error-whole-messages
        error-warning-messages
        warning-messages
        predspecs
        trace-exit
        trace-fail
        trace-redo
        trace-call
        trace-exception))
      ((eq major-mode 'compilation-mode)
       (list
        error-message-identifier
        error-whole-messages
        error-warning-messages
        warning-messages
        predspecs))))
    ))



(defun wild_life-mode-find-unmatched-paren ()
  "Return the column of the last unmatched left parenthesis."
  (save-excursion
    (goto-char (or (car (nth 9 (syntax-ppss))) (point-min)))
    (current-column)))


(defun wild_life-mode-paren-balance ()
  "Return the parenthesis balance of the current line.
A return value of N means N more left parentheses than right ones."
  (save-excursion
    (car (parse-partial-sexp (line-beginning-position)
                             (line-end-position)))))

(defun wild_life-mode-electric--if-then-else ()
  "Insert spaces after the opening parenthesis, \"then\" (->) and \"else\" (;) branches.
Spaces are inserted if all preceding objects on the line are
whitespace characters, parentheses, or then/else branches."
  (when wild_life-mode-electric-if-then-else-flag
    (save-excursion
      (let ((regexp (concat "(\\|" wild_life-mode-left-indent-regexp))
            level)
        (beginning-of-line)
        (skip-chars-forward " \t")
        ;; Treat "( If -> " lines specially.
        ;;(setq incr (if (looking-at "(.*->")
        ;;               2
        ;;             wild_life-mode-paren-indent))

        ;; work on all subsequent "->", "(", ";"
        (while (looking-at regexp)
          (goto-char (match-end 0))
          (setq level (+ (wild_life-mode-find-unmatched-paren) wild_life-mode-paren-indent))

          ;; Remove old white space
          (let ((start (point)))
            (skip-chars-forward " \t")
            (delete-region start (point)))
          (indent-to level)
          (skip-chars-forward " \t"))
        ))
    (when (save-excursion
            (backward-char 2)
            (looking-at "\\s ;\\|\\s (\\|->")) ; (looking-at "\\s \\((\\|;\\)"))
      (skip-chars-forward " \t"))
    ))

;;;; Comment filling

(defun wild_life-mode-comment-limits ()
  "Return the current comment limits plus the comment type (block or line).
The comment limits are the range of a block comment or the range that
contains all adjacent line comments (i.e. all comments that starts in
the same column with no empty lines or non-whitespace characters
between them)."
  (let ((here (point))
        lit-limits-b lit-limits-e lit-type beg end
        )
    (save-restriction
      ;; Widen to catch comment limits correctly.
      (widen)
      (setq end (line-end-position)
            beg (line-beginning-position))
      (save-excursion
        (beginning-of-line)
        (setq lit-type (if (search-forward-regexp "%" end t) 'line 'block))
                        ;    (setq lit-type 'line)
                        ;(if (search-forward-regexp "^[ \t]*%" end t)
                        ;    (setq lit-type 'line)
                        ;  (if (not (search-forward-regexp "%" end t))
                        ;      (setq lit-type 'block)
                        ;    (if (not (= (forward-line 1) 0))
                        ;        (setq lit-type 'block)
                        ;      (setq done t
                        ;            ret (wild_life-mode-comment-limits)))
                        ;    ))
        (if (eq lit-type 'block)
            (progn
              (goto-char here)
              (when (looking-at "/\\*") (forward-char 2))
              (when (and (looking-at "\\*") (> (point) (point-min))
                         (forward-char -1) (looking-at "/"))
                (forward-char 1))
              (when (save-excursion (search-backward "/*" nil t))
                (list (save-excursion (search-backward "/*") (point))
                      (or (search-forward "*/" nil t) (point-max)) lit-type)))
          ;; line comment
          (setq lit-limits-b (- (point) 1)
                lit-limits-e end)
          (condition-case nil
              (if (progn (goto-char lit-limits-b)
                         (looking-at "%"))
                  (let ((col (current-column)) done)
                    (setq beg (point)
                          end lit-limits-e)
                    ;; Always at the beginning of the comment
                    ;; Go backward now
                    (beginning-of-line)
                    (while (and (zerop (setq done (forward-line -1)))
                                (search-forward-regexp "^[ \t]*%"
                                                       (line-end-position) t)
                                (= (+ 1 col) (current-column)))
                      (setq beg (- (point) 1)))
                    (when (= done 0)
                      (forward-line 1))
                    ;; We may have a line with code above...
                    (when (and (zerop (setq done (forward-line -1)))
                               (search-forward "%" (line-end-position) t)
                               (= (+ 1 col) (current-column)))
                      (setq beg (- (point) 1)))
                    (when (= done 0)
                      (forward-line 1))
                    ;; Go forward
                    (goto-char lit-limits-b)
                    (beginning-of-line)
                    (while (and (zerop (forward-line 1))
                                (search-forward-regexp "^[ \t]*%"
                                                       (line-end-position) t)
                                (= (+ 1 col) (current-column)))
                      (setq end (line-end-position)))
                    (list beg end lit-type))
                (list lit-limits-b lit-limits-e lit-type)
                )
            (error (list lit-limits-b lit-limits-e lit-type))))
        ))))

(defun wild_life-mode-guess-fill-prefix ()
  ;; fill 'txt entities?
  (when (save-excursion
          (end-of-line)
          (nth 4 (syntax-ppss)))
    (let* ((bounds (wild_life-mode-comment-limits))
           (cbeg (car bounds))
           (type (nth 2 bounds))
           beg end)
      (save-excursion
        (end-of-line)
        (setq end (point))
        (beginning-of-line)
        (setq beg (point))
        (if (and (eq type 'line)
                 (> cbeg beg)
                 (save-excursion (not (search-forward-regexp "^[ \t]*%"
                                                             cbeg t))))
            (progn
              (goto-char cbeg)
              (search-forward-regexp "%+[ \t]*" end t)
              (wild_life-mode-replace-in-string (buffer-substring beg (point))
                                        "[^ \t%]" " "))
          ;(goto-char beg)
          (if (search-forward-regexp "^[ \t]*\\(%+\\|\\*+\\|/\\*+\\)[ \t]*"
                                     end t)
              (wild_life-mode-replace-in-string (buffer-substring beg (point)) "/" " ")
            (beginning-of-line)
            (when (search-forward-regexp "^[ \t]+" end t)
              (buffer-substring beg (point)))))))))

(defun wild_life-mode-fill-paragraph ()
  "Fill paragraph comment at or after point."
  (interactive)
  (let* ((bounds (wild_life-mode-comment-limits))
         (type (nth 2 bounds)))
    (if (eq type 'line)
        (let ((fill-prefix (wild_life-mode-guess-fill-prefix)))
          (fill-paragraph nil))
      (save-excursion
        (save-restriction
          ;; exclude surrounding lines that delimit a multiline comment
          ;; and don't contain alphabetic characters, like "/*******",
          ;; "- - - */" etc.
          (save-excursion
            (backward-paragraph)
            (unless (bobp) (forward-line))
            (if (string-match "^/\\*[^a-zA-Z]*$" (thing-at-point 'line))
                (narrow-to-region (point-at-eol) (point-max))))
          (save-excursion
            (forward-paragraph)
            (forward-line -1)
            (if (string-match "^[^a-zA-Z]*\\*/$" (thing-at-point 'line))
                (narrow-to-region (point-min) (point-at-bol))))
          (let ((fill-prefix (wild_life-mode-guess-fill-prefix)))
            (fill-paragraph nil))))
      )))

(defun wild_life-mode-do-auto-fill ()
  "Carry out Auto Fill for Wild_Life-Mode mode.
In effect it sets the `fill-prefix' when inside comments and then calls
`do-auto-fill'."
  (let ((fill-prefix (wild_life-mode-guess-fill-prefix)))
    (do-auto-fill)
    ))

(defalias 'wild_life-mode-replace-in-string
  (if (fboundp 'replace-in-string)
      #'replace-in-string
    (lambda (str regexp newtext &optional literal)
      (replace-regexp-in-string regexp newtext str nil literal))))

;;-------------------------------------------------------------------
;; Online help
;;-------------------------------------------------------------------

(defvar wild_life-mode-help-function
  '((mercury nil)
    (eclipse wild_life-mode-help-online)
    ;; (sicstus wild_life-mode-help-info)
    (sicstus wild_life-mode-find-documentation)
    (swi wild_life-mode-help-online)
    (t wild_life-mode-help-online))
  "Alist for the name of the function for finding help on a predicate.")

(defun wild_life-mode-help-on-predicate ()
  "Invoke online help on the atom under cursor."
  (interactive)

  (cond
   ;; Redirect help for SICStus to `wild_life-mode-find-documentation'.
   ((eq wild_life-mode-help-function-i 'wild_life-mode-find-documentation)
    (wild_life-mode-find-documentation))

   ;; Otherwise, ask for the predicate name and then call the function
   ;; in wild_life-mode-help-function-i
   (t
    (let* ((word (wild_life-mode-atom-under-point))
           (predicate (read-string
                       (format "Help on predicate%s: "
                               (if word
                                   (concat " (default " word ")")
                                 ""))
                       nil nil word))
           ;;point
           )
      (if wild_life-mode-help-function-i
          (funcall wild_life-mode-help-function-i predicate)
        (error "Sorry, no help method defined for this Wild_Life-Mode system."))))
   ))


(autoload 'Info-goto-node "info" nil t)
(declare-function Info-follow-nearest-node "info" (&optional FORK))

(defun wild_life-mode-help-info (predicate)
  (let ((buffer (current-buffer))
        oldp
        (str (concat "^\\* " (regexp-quote predicate) " */")))
    (pop-to-buffer nil)
    (Info-goto-node wild_life-mode-info-predicate-index)
    (if (not (re-search-forward str nil t))
        (error (format "Help on predicate `%s' not found." predicate)))

    (setq oldp (point))
    (if (re-search-forward str nil t)
        ;; Multiple matches, ask user
        (let ((max 2)
              n)
          ;; Count matches
          (while (re-search-forward str nil t)
            (setq max (1+ max)))

          (goto-char oldp)
          (re-search-backward "[^ /]" nil t)
          (recenter 0)
          (setq n (read-string  ;; was read-input, which is obsolete
                   (format "Several matches, choose (1-%d): " max) "1"))
          (forward-line (- (string-to-number n) 1)))
      ;; Single match
      (re-search-backward "[^ /]" nil t))

    ;; (Info-follow-nearest-node (point))
    (wild_life-mode-Info-follow-nearest-node)
    (re-search-forward (concat "^`" (regexp-quote predicate)) nil t)
    (beginning-of-line)
    (recenter 0)
    (pop-to-buffer buffer)))

(defun wild_life-mode-Info-follow-nearest-node ()
  (if (featurep 'xemacs)
      (Info-follow-nearest-node (point))
    (Info-follow-nearest-node)))

(defun wild_life-mode-help-online (predicate)
  (wild_life-mode-ensure-process)
  (process-send-string "wild_life-mode" (concat "help(" predicate ").\n"))
  (display-buffer "*wild_life-mode*"))

(defun wild_life-mode-help-apropos (string)
  "Find Wild_Life-Mode apropos on given STRING.
This function is only available when `wild_life-mode-system' is set to `swi'."
  (interactive "sApropos: ")
  (cond
   ((eq wild_life-mode-system 'swi)
    (wild_life-mode-ensure-process)
    (process-send-string "wild_life-mode" (concat "apropos(" string ").\n"))
    (display-buffer "*wild_life-mode*"))
   (t
    (error "Sorry, no Wild_Life-Mode apropos available for this Wild_Life-Mode system."))))

(defun wild_life-mode-atom-under-point ()
  "Return the atom under or left to the point."
  (save-excursion
    (let ((nonatom_chars "[](){},\. \t\n")
          start)
      (skip-chars-forward (concat "^" nonatom_chars))
      (skip-chars-backward nonatom_chars)
      (skip-chars-backward (concat "^" nonatom_chars))
      (setq start (point))
      (skip-chars-forward (concat "^" nonatom_chars))
      (buffer-substring-no-properties start (point))
      )))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Help function with completion
;; Stolen from Per Mildner's SICStus debugger mode and modified

(defun wild_life-mode-find-documentation ()
  "Go to the Info node for a predicate in the SICStus Info manual."
  (interactive)
  (let ((pred (wild_life-mode-read-predicate)))
    (wild_life-mode-goto-predicate-info pred)))

(defvar wild_life-mode-info-alist nil
  "Alist with all builtin predicates.
Only for internal use by `wild_life-mode-find-documentation'")

;; Very similar to wild_life-mode-help-info except that that function cannot
;; cope with arity and that it asks the user if there are several
;; functors with different arity. This function also uses
;; wild_life-mode-info-alist for finding the info node, rather than parsing
;; the predicate index.
(defun wild_life-mode-goto-predicate-info (predicate)
  "Go to the info page for PREDICATE, which is a PredSpec."
  (interactive)
  (string-match "\\(.*\\)/\\([0-9]+\\).*$" predicate)
  (let ((buffer (current-buffer))
        (name (match-string 1 predicate))
        (arity (string-to-number (match-string 2 predicate)))
        ;oldp
        ;(str (regexp-quote predicate))
        )
    (pop-to-buffer nil)

    (Info-goto-node
     wild_life-mode-info-predicate-index) ;; We must be in the SICStus pages
    (Info-goto-node (car (cdr (assoc predicate wild_life-mode-info-alist))))

    (wild_life-mode-find-term (regexp-quote name) arity "^`")

    (recenter 0)
    (pop-to-buffer buffer))
)

(defun wild_life-mode-read-predicate ()
  "Read a PredSpec from the user.
Returned value is a string \"FUNCTOR/ARITY\".
Interaction supports completion."
  (let ((default (wild_life-mode-atom-under-point)))
    ;; If the predicate index is not yet built, do it now
    (if (not wild_life-mode-info-alist)
        (wild_life-mode-build-info-alist))
    ;; Test if the default string could be the base for completion.
    ;; Discard it if not.
    (if (eq (try-completion default wild_life-mode-info-alist) nil)
        (setq default nil))
    ;; Read the PredSpec from the user
    (completing-read
     (if (zerop (length default))
         "Help on predicate: "
       (concat "Help on predicate (default " default "): "))
     wild_life-mode-info-alist nil t nil nil default)))

(defun wild_life-mode-build-info-alist (&optional verbose)
  "Build an alist of all builtins and library predicates.
Each element is of the form (\"NAME/ARITY\" . (INFO-NODE1 INFO-NODE2 ...)).
Typically there is just one Info node associated with each name
If an optional argument VERBOSE is non-nil, print messages at the beginning
and end of list building."
  (if verbose
      (message "Building info alist..."))
  (setq wild_life-mode-info-alist
        (let ((l ())
              (last-entry (cons "" ())))
          (save-excursion
            (save-window-excursion
              ;; select any window but the minibuffer (as we cannot switch
              ;; buffers in minibuffer window.
              ;; I am not sure this is the right/best way
              (if (active-minibuffer-window)  ; nil if none active
                  (select-window (next-window)))
              ;; Do this after going away from minibuffer window
              (save-window-excursion
                (info))
              (Info-goto-node wild_life-mode-info-predicate-index)
              (goto-char (point-min))
              (while (re-search-forward
                      "^\\* \\(.+\\)/\\([0-9]+\\)\\([^\n:*]*\\):" nil t)
                (let* ((name (match-string 1))
                       (arity (string-to-number (match-string 2)))
                       (comment (match-string 3))
                       (fa (format "%s/%d%s" name arity comment))
                       info-node)
                  (beginning-of-line)
                  ;; Extract the info node name
                  (setq info-node (progn
                                    (re-search-forward ":[ \t]*\\([^:]+\\).$")
                                    (match-string 1)
                                   ))
                  ;; ###### Easier? (from Milan version 0.1.28)
                  ;; (setq info-node (Info-extract-menu-node-name))
                  (if (equal fa (car last-entry))
                      (setcdr last-entry (cons info-node (cdr last-entry)))
                    (setq last-entry (cons fa (list info-node))
                          l (cons last-entry l)))))
              (nreverse l)
              ))))
  (if verbose
      (message "Building info alist... done.")))


;;-------------------------------------------------------------------
;; Miscellaneous functions
;;-------------------------------------------------------------------

;; For Windows. Change backslash to slash. SICStus handles either
;; path separator but backslash must be doubled, therefore use slash.
(defun wild_life-mode-bsts (string)
  "Change backslashes to slashes in STRING."
  (let ((str1 (copy-sequence string))
        (len (length string))
        (i 0))
    (while (< i len)
      (if (char-equal (aref str1 i) ?\\)
          (aset str1 i ?/))
      (setq i (1+ i)))
    str1))

;;(defun wild_life-mode-temporary-file ()
;;  "Make temporary file name for compilation."
;;  (make-temp-name
;;   (concat
;;    (or
;;     (getenv "TMPDIR")
;;     (getenv "TEMP")
;;     (getenv "TMP")
;;     (getenv "SYSTEMP")
;;     "/tmp")
;;    "/prolcomp")))
;;(setq wild_life-mode-temp-filename (wild_life-mode-bsts (wild_life-mode-temporary-file)))

(defun wild_life-mode-temporary-file ()
  "Make temporary file name for compilation."
  (if wild_life-mode-temporary-file-name
      ;; We already have a file, erase content and continue
      (progn
        (write-region "" nil wild_life-mode-temporary-file-name nil 'silent)
        wild_life-mode-temporary-file-name)
    ;; Actually create the file and set `wild_life-mode-temporary-file-name'
    ;; accordingly.
    (setq wild_life-mode-temporary-file-name
          (make-temp-file "prolcomp" nil ".lf"))))

(defun wild_life-mode-goto-wild_life-mode-process-buffer ()
  "Switch to the wild_life-mode process buffer and go to its end."
  (switch-to-buffer-other-window "*wild_life-mode*")
  (goto-char (point-max))
)

(defun wild_life-mode-enable-sicstus-sd ()
  "Enable the source level debugging facilities of SICStus 3.7 and later."
  (interactive)
  (require 'pltrace)  ; Load the SICStus debugger code
  ;; Turn on the source level debugging by default
  (add-hook 'wild_life-mode-inferior-mode-hook 'pltrace-on)
  (if (not wild_life-mode-use-sicstus-sd)
      (progn
        ;; If there is a *wild_life-mode* buffer, then call pltrace-on
        (if (get-buffer "*wild_life-mode*")
            ;; Avoid compilation warnings by using eval
            (eval '(pltrace-on)))
        (setq wild_life-mode-use-sicstus-sd t)
        )))

(defun wild_life-mode-disable-sicstus-sd ()
  "Disable the source level debugging facilities of SICStus 3.7 and later."
  (interactive)
  (setq wild_life-mode-use-sicstus-sd nil)
  ;; Remove the hook
  (remove-hook 'wild_life-mode-inferior-mode-hook 'pltrace-on)
  ;; If there is a *wild_life-mode* buffer, then call pltrace-off
  (if (get-buffer "*wild_life-mode*")
      ;; Avoid compile warnings by using eval
      (eval '(pltrace-off))))

(defun wild_life-mode-toggle-sicstus-sd ()
  ;; FIXME: Use define-minor-mode.
  "Toggle the source level debugging facilities of SICStus 3.7 and later."
  (interactive)
  (if wild_life-mode-use-sicstus-sd
      (wild_life-mode-disable-sicstus-sd)
    (wild_life-mode-enable-sicstus-sd)))

(defun wild_life-mode-debug-on (&optional arg)
  "Enable debugging.
When called with prefix argument ARG, disable debugging instead."
  (interactive "P")
  (if arg
      (wild_life-mode-debug-off)
    (wild_life-mode-process-insert-string (get-process "wild_life-mode")
                                  wild_life-mode-debug-on-string)
    (process-send-string "wild_life-mode" wild_life-mode-debug-on-string)))

(defun wild_life-mode-debug-off ()
  "Disable debugging."
  (interactive)
  (wild_life-mode-process-insert-string (get-process "wild_life-mode")
                                wild_life-mode-debug-off-string)
  (process-send-string "wild_life-mode" wild_life-mode-debug-off-string))

(defun wild_life-mode-trace-on (&optional arg)
  "Enable tracing.
When called with prefix argument ARG, disable tracing instead."
  (interactive "P")
  (if arg
      (wild_life-mode-trace-off)
    (wild_life-mode-process-insert-string (get-process "wild_life-mode")
                                  wild_life-mode-trace-on-string)
    (process-send-string "wild_life-mode" wild_life-mode-trace-on-string)))

(defun wild_life-mode-trace-off ()
  "Disable tracing."
  (interactive)
  (wild_life-mode-process-insert-string (get-process "wild_life-mode")
                                wild_life-mode-trace-off-string)
  (process-send-string "wild_life-mode" wild_life-mode-trace-off-string))

(defun wild_life-mode-zip-on (&optional arg)
  "Enable zipping (for SICStus 3.7 and later).
When called with prefix argument ARG, disable zipping instead."
  (interactive "P")
  (if (not (and (eq wild_life-mode-system 'sicstus)
                (wild_life-mode-atleast-version '(3 . 7))))
      (error "Only works for SICStus 3.7 and later"))
  (if arg
      (wild_life-mode-zip-off)
    (wild_life-mode-process-insert-string (get-process "wild_life-mode")
                                  wild_life-mode-zip-on-string)
    (process-send-string "wild_life-mode" wild_life-mode-zip-on-string)))

(defun wild_life-mode-zip-off ()
  "Disable zipping (for SICStus 3.7 and later)."
  (interactive)
  (wild_life-mode-process-insert-string (get-process "wild_life-mode")
                                wild_life-mode-zip-off-string)
  (process-send-string "wild_life-mode" wild_life-mode-zip-off-string))

;; (defun wild_life-mode-create-predicate-index ()
;;   "Create an index for all predicates in the buffer."
;;   (let ((predlist '())
;;         clauseinfo
;;         object
;;         pos
;;         )
;;     (goto-char (point-min))
;;     ;; Replace with wild_life-mode-clause-start!
;;     (while (re-search-forward "^.+:-" nil t)
;;       (setq pos (match-beginning 0))
;;       (setq clauseinfo (wild_life-mode-clause-info))
;;       (setq object (wild_life-mode-in-object))
;;       (setq predlist (append
;;                       predlist
;;                       (list (cons
;;                              (if (and (eq wild_life-mode-system 'sicstus)
;;                                       (wild_life-mode-in-object))
;;                                  (format "%s::%s/%d"
;;                                          object
;;                                          (nth 0 clauseinfo)
;;                                          (nth 1 clauseinfo))
;;                                (format "%s/%d"
;;                                        (nth 0 clauseinfo)
;;                                        (nth 1 clauseinfo)))
;;                              pos
;;                              ))))
;;       (wild_life-mode-end-of-predicate))
;;     predlist))

(defun wild_life-mode-get-predspec ()
  (save-excursion
    (let ((state (wild_life-mode-clause-info))
          (object (wild_life-mode-in-object)))
      (if (or (equal (nth 0 state) "")
              (nth 4 (syntax-ppss)))
          nil
        (if (and (eq wild_life-mode-system 'sicstus)
                 object)
            (format "%s::%s/%d"
                    object
                    (nth 0 state)
                    (nth 1 state))
          (format "%s/%d"
                  (nth 0 state)
                  (nth 1 state)))
        ))))

;; For backward compatibility. Stolen from custom.el.
(or (fboundp 'match-string)
    ;; Introduced in Emacs 19.29.
    (defun match-string (num &optional string)
  "Return string of text matched by last search.
NUM specifies which parenthesized expression in the last regexp.
 Value is nil if NUMth pair didn't match, or there were less than NUM pairs.
Zero means the entire text matched by the whole regexp or whole string.
STRING should be given if the last search was by `string-match' on STRING."
  (if (match-beginning num)
      (if string
          (substring string (match-beginning num) (match-end num))
        (buffer-substring (match-beginning num) (match-end num))))))

(defun wild_life-mode-pred-start ()
  "Return the starting point of the first clause of the current predicate."
  ;; FIXME: Use SMIE.
  (save-excursion
    (goto-char (wild_life-mode-clause-start))
    ;; Find first clause, unless it was a directive
    (if (and (not (looking-at "[:?]-"))
             (not (looking-at "[ \t]*[%/]"))  ; Comment

             )
        (let* ((pinfo (wild_life-mode-clause-info))
               (predname (nth 0 pinfo))
               (arity (nth 1 pinfo))
               (op (point)))
          (while (and (re-search-backward
                       (format "^%s\\([(\\.]\\| *%s\\)"
                               predname wild_life-mode-head-delimiter) nil t)
                      (= arity (nth 1 (wild_life-mode-clause-info)))
                      )
            (setq op (point)))
          (if (eq wild_life-mode-system 'mercury)
              ;; Skip to the beginning of declarations of the predicate
              (progn
                (goto-char (wild_life-mode-beginning-of-clause))
                (while (and (not (eq (point) op))
                            (looking-at
                             (format ":-[ \t]*\\(pred\\|mode\\)[ \t]+%s"
                                     predname)))
                  (setq op (point))
                  (goto-char (wild_life-mode-beginning-of-clause)))))
          op)
      (point))))

(defun wild_life-mode-pred-end ()
  "Return the position at the end of the last clause of the current predicate."
  ;; FIXME: Use SMIE.
  (save-excursion
    (goto-char (wild_life-mode-clause-end))     ; If we are before the first predicate.
    (goto-char (wild_life-mode-clause-start))
    (let* ((pinfo (wild_life-mode-clause-info))
          (predname (nth 0 pinfo))
          (arity (nth 1 pinfo))
          oldp
          (notdone t)
          (op (point)))
      (if (looking-at "[:?]-")
          ;; This was a directive
          (progn
            (if (and (eq wild_life-mode-system 'mercury)
                     (looking-at
                      (format ":-[ \t]*\\(pred\\|mode\\)[ \t]+\\(%s+\\)"
                              wild_life-mode-atom-regexp)))
                ;; Skip predicate declarations
                (progn
                  (setq predname (buffer-substring-no-properties
                                  (match-beginning 2) (match-end 2)))
                  (while (re-search-forward
                          (format
                           "\n*\\(:-[ \t]*\\(pred\\|mode\\)[ \t]+\\)?%s[( \t]"
                           predname)
                          nil t))))
            (goto-char (wild_life-mode-clause-end))
            (setq op (point)))
        ;; It was not a directive, find the last clause
        (while (and notdone
                    (re-search-forward
                     (format "^%s\\([(\\.]\\| *%s\\)"
                             predname wild_life-mode-head-delimiter) nil t)
                    (= arity (nth 1 (wild_life-mode-clause-info))))
          (setq oldp (point))
          (setq op (wild_life-mode-clause-end))
          (if (>= oldp op)
              ;; End of clause not found.
              (setq notdone nil)
            ;; Continue while loop
            (goto-char op))))
      op)))

(defun wild_life-mode-clause-start (&optional not-allow-methods)
  "Return the position at the start of the head of the current clause.
If NOTALLOWMETHODS is non-nil then do not match on methods in
objects (relevant only if `wild_life-mode-system' is set to `sicstus')."
  (save-excursion
    (let ((notdone t)
          (retval (point-min)))
      (end-of-line)

      ;; SICStus object?
      (if (and (not not-allow-methods)
               (eq wild_life-mode-system 'sicstus)
               (wild_life-mode-in-object))
          (while (and
                  notdone
                  ;; Search for a head or a fact
                  (re-search-backward
                   ;; If in object, then find method start.
                   ;; "^[ \t]+[a-z$].*\\(:-\\|&\\|:: {\\|,\\)"
                   "^[ \t]+[a-z$].*\\(:-\\|&\\|:: {\\)" ; The comma causes
                                        ; problems since we cannot assume
                                        ; that the line starts at column 0,
                                        ; thus we don't know if the line
                                        ; is a head or a subgoal
                   (point-min) t))
            (if (>= (wild_life-mode-paren-balance) 0) ; To no match on "   a) :-"
                ;; Start of method found
                (progn
                  (setq retval (point))
                  (setq notdone nil)))
            )                                ; End of while

        ;; Not in object
        (while (and
                notdone
                ;; Search for a text at beginning of a line
                ;; ######
                ;; (re-search-backward "^[a-z$']" nil t))
                (let ((case-fold-search nil))
                  (re-search-backward "^\\([[:lower:]$']\\|[:?]-\\)"
                                      nil t)))
          (let ((bal (wild_life-mode-paren-balance)))
            (cond
             ((> bal 0)
              ;; Start of clause found
              (progn
                (setq retval (point))
                (setq notdone nil)))
             ((and (= bal 0)
                   (looking-at
                    (format ".*\\(\\.\\|%s\\|!,\\)[ \t]*\\(%%.*\\|\\)$"
                            wild_life-mode-head-delimiter)))
              ;; Start of clause found if the line ends with a '.' or
              ;; a wild_life-mode-head-delimiter
              (progn
                (setq retval (point))
                (setq notdone nil))
              )
             (t nil) ; Do nothing
             ))))

        retval)))

(defun wild_life-mode-clause-end (&optional not-allow-methods)
  "Return the position at the end of the current clause.
If NOTALLOWMETHODS is non-nil then do not match on methods in
objects (relevant only if `wild_life-mode-system' is set to `sicstus')."
  (save-excursion
    (beginning-of-line) ; Necessary since we use "^...." for the search.
    (if (re-search-forward
         (if (and (not not-allow-methods)
                  (eq wild_life-mode-system 'sicstus)
                  (wild_life-mode-in-object))
             (format
              "^\\(%s\\|%s\\|[^\n\'\"%%]\\)*&[ \t]*\\(\\|%%.*\\)$\\|[ \t]*}"
              wild_life-mode-quoted-atom-regexp wild_life-mode-string-regexp)
           (format
            "^\\(%s\\|%s\\|[^\n\'\"%%]\\)*\\.[ \t]*\\(\\|%%.*\\)$"
            wild_life-mode-quoted-atom-regexp wild_life-mode-string-regexp))
         nil t)
        (if (and (nth 8 (syntax-ppss))
                 (not (eobp)))
            (progn
              (forward-char)
              (wild_life-mode-clause-end))
          (point))
      (point))))

(defun wild_life-mode-clause-info ()
  "Return a (name arity) list for the current clause."
  (save-excursion
    (goto-char (wild_life-mode-clause-start))
    (let* ((op (point))
           (predname
            (if (looking-at wild_life-mode-atom-char-regexp)
                (progn
                  (skip-chars-forward "^ (\\.")
                  (buffer-substring op (point)))
              ""))
           (arity 0))
      ;; Retrieve the arity.
      (if (looking-at wild_life-mode-left-paren)
          (let ((endp (save-excursion
                        (forward-list) (point))))
            (setq arity 1)
            (forward-char 1)            ; Skip the opening paren.
            (while (progn
                     (skip-chars-forward "^[({,'\"")
                     (< (point) endp))
              (if (looking-at ",")
                  (progn
                    (setq arity (1+ arity))
                    (forward-char 1)    ; Skip the comma.
                    )
                ;; We found a string, list or something else we want
                ;; to skip over.
                (forward-sexp 1))
              )))
      (list predname arity))))

(defun wild_life-mode-in-object ()
  "Return object name if the point is inside a SICStus object definition."
  ;; Return object name if the last line that starts with a character
  ;; that is neither white space nor a comment start
  (save-excursion
    (if (save-excursion
          (beginning-of-line)
          (looking-at "\\([^\n ]+\\)[ \t]*::[ \t]*{"))
        ;; We were in the head of the object
        (match-string 1)
      ;; We were not in the head
      (if (and (re-search-backward "^[a-z$'}]" nil t)
               (looking-at "\\([^\n ]+\\)[ \t]*::[ \t]*{"))
          (match-string 1)
        nil))))

(defun wild_life-mode-beginning-of-clause ()
  "Move to the beginning of current clause.
If already at the beginning of clause, move to previous clause."
  (interactive)
  (let ((point (point))
        (new-point (wild_life-mode-clause-start)))
    (if (and (>= new-point point)
             (> point 1))
        (progn
          (goto-char (1- point))
          (goto-char (wild_life-mode-clause-start)))
      (goto-char new-point)
      (skip-chars-forward " \t"))))

;; (defun wild_life-mode-previous-clause ()
;;   "Move to the beginning of the previous clause."
;;   (interactive)
;;   (forward-char -1)
;;   (wild_life-mode-beginning-of-clause))

(defun wild_life-mode-end-of-clause ()
  "Move to the end of clause.
If already at the end of clause, move to next clause."
  (interactive)
  (let ((point (point))
        (new-point (wild_life-mode-clause-end)))
    (if (and (<= new-point point)
             (not (eq new-point (point-max))))
        (progn
          (goto-char (1+ point))
          (goto-char (wild_life-mode-clause-end)))
      (goto-char new-point))))

;; (defun wild_life-mode-next-clause ()
;;   "Move to the beginning of the next clause."
;;   (interactive)
;;   (wild_life-mode-end-of-clause)
;;   (forward-char)
;;   (wild_life-mode-end-of-clause)
;;   (wild_life-mode-beginning-of-clause))

(defun wild_life-mode-beginning-of-predicate ()
  "Go to the nearest beginning of predicate before current point.
Return the final point or nil if no such a beginning was found."
  ;; FIXME: Hook into beginning-of-defun.
  (interactive)
  (let ((op (point))
        (pos (wild_life-mode-pred-start)))
    (if pos
        (if (= op pos)
            (if (not (bobp))
                (progn
                  (goto-char pos)
                  (backward-char 1)
                  (setq pos (wild_life-mode-pred-start))
                  (if pos
                      (progn
                        (goto-char pos)
                        (point)))))
          (goto-char pos)
          (point)))))

(defun wild_life-mode-end-of-predicate ()
  "Go to the end of the current predicate."
  ;; FIXME: Hook into end-of-defun.
  (interactive)
  (let ((op (point)))
    (goto-char (wild_life-mode-pred-end))
    (if (= op (point))
        (progn
          (forward-line 1)
          (wild_life-mode-end-of-predicate)))))

(defun wild_life-mode-insert-predspec ()
  "Insert the predspec for the current predicate."
  (interactive)
  (let* ((pinfo (wild_life-mode-clause-info))
         (predname (nth 0 pinfo))
         (arity (nth 1 pinfo)))
    (insert (format "%s/%d" predname arity))))

(defun wild_life-mode-view-predspec ()
  "Insert the predspec for the current predicate."
  (interactive)
  (let* ((pinfo (wild_life-mode-clause-info))
         (predname (nth 0 pinfo))
         (arity (nth 1 pinfo)))
    (message (format "%s/%d" predname arity))))

(defun wild_life-mode-insert-predicate-template ()
  "Insert the template for the current clause."
  (interactive)
  (let* ((n 1)
         oldp
         (pinfo (wild_life-mode-clause-info))
         (predname (nth 0 pinfo))
         (arity (nth 1 pinfo)))
    (insert predname)
    (if (> arity 0)
        (progn
          (insert "(")
 	  (when wild_life-mode-electric-dot-full-predicate-template
 	    (setq oldp (point))
 	    (while (< n arity)
 	      (insert ",")
 	      (setq n (1+ n)))
 	    (insert ")")
 	    (goto-char oldp))
          ))
  ))

(defun wild_life-mode-insert-next-clause ()
  "Insert newline and the name of the current clause."
  (interactive)
  (insert "\n")
  (wild_life-mode-insert-predicate-template))

(defun wild_life-mode-insert-module-modeline ()
  "Insert a modeline for module specification.
This line should be first in the buffer.
The module name should be written manually just before the semi-colon."
  (interactive)
  (insert "%%% -*- Module: ; -*-\n")
  (backward-char 6))

(defalias 'wild_life-mode-uncomment-region
  (if (fboundp 'uncomment-region) #'uncomment-region
    (lambda (beg end)
      "Uncomment the region between BEG and END."
      (interactive "r")
      (comment-region beg end -1))))

(defun wild_life-mode-indent-predicate ()
  "Indent the current predicate."
  (interactive)
  (indent-region (wild_life-mode-pred-start) (wild_life-mode-pred-end) nil))

(defun wild_life-mode-indent-buffer ()
  "Indent the entire buffer."
  (interactive)
  (indent-region (point-min) (point-max) nil))

(defun wild_life-mode-mark-clause ()
  "Put mark at the end of this clause and move point to the beginning."
  (interactive)
  (let ((pos (point)))
    (goto-char (wild_life-mode-clause-end))
    (forward-line 1)
    (beginning-of-line)
    (set-mark (point))
    (goto-char pos)
    (goto-char (wild_life-mode-clause-start))))

(defun wild_life-mode-mark-predicate ()
  "Put mark at the end of this predicate and move point to the beginning."
  (interactive)
  (goto-char (wild_life-mode-pred-end))
  (let ((pos (point)))
    (forward-line 1)
    (beginning-of-line)
    (set-mark (point))
    (goto-char pos)
    (goto-char (wild_life-mode-pred-start))))

(defun wild_life-mode-electric--colon ()
  "If `wild_life-mode-electric-colon-flag' is non-nil, insert the electric `:' construct.
That is, insert space (if appropriate), `:-' and newline if colon is pressed
at the end of a line that starts in the first column (i.e., clause heads)."
  (when (and wild_life-mode-electric-colon-flag
             (eq (char-before) ?:)
             (not current-prefix-arg)
             (eolp)
             (not (memq (char-after (line-beginning-position))
                        '(?\s ?\t ?\%))))
    (unless (memq (char-before (1- (point))) '(?\s ?\t))
      (save-excursion (forward-char -1) (insert " ")))
    (insert "-\n")
    (indent-according-to-mode)))

(defun wild_life-mode-electric--dash ()
  "If `wild_life-mode-electric-dash-flag' is non-nil, insert the electric `-' construct.
that is, insert space (if appropriate), `-->' and newline if dash is pressed
at the end of a line that starts in the first column (i.e., DCG heads)."
  (when (and wild_life-mode-electric-dash-flag
             (eq (char-before) ?-)
             (not current-prefix-arg)
             (eolp)
             (not (memq (char-after (line-beginning-position))
                        '(?\s ?\t ?\%))))
    (unless (memq (char-before (1- (point))) '(?\s ?\t))
      (save-excursion (forward-char -1) (insert " ")))
    (insert "->\n")
    (indent-according-to-mode)))

(defun wild_life-mode-electric--dot ()
  "Make dot electric, if `wild_life-mode-electric-dot-flag' is non-nil.
When invoked at the end of nonempty line, insert dot and newline.
When invoked at the end of an empty line, insert a recursive call to
the current predicate.
When invoked at the beginning of line, insert a head of a new clause
of the current predicate."
  ;; Check for situations when the electricity should not be active
  (if (or (not wild_life-mode-electric-dot-flag)
          (not (eq (char-before) ?\.))
          current-prefix-arg
          (nth 8 (syntax-ppss))
          ;; Do not be electric in a floating point number or an operator
          (not
           (save-excursion
             (forward-char -1)
             (skip-chars-backward " \t")
             (let ((num (> (skip-chars-backward "0-9") 0)))
               (or (bolp)
                   (memq (char-syntax (char-before))
                         (if num '(?w ?_) '(?\) ?w ?_)))))))
          ;; Do not be electric if inside a parenthesis pair.
          (not (= (car (syntax-ppss))
                  0))
          )
      nil ;;Not electric.
    (cond
     ;; Beginning of line
     ((save-excursion (forward-char -1) (bolp))
      (delete-region (1- (point)) (point)) ;Delete the dot that called us.
      (wild_life-mode-insert-predicate-template))
     ;; At an empty line with at least one whitespace
     ((save-excursion
        (beginning-of-line)
        (looking-at "[ \t]+\\.$"))
      (delete-region (1- (point)) (point)) ;Delete the dot that called us.
      (wild_life-mode-insert-predicate-template)
      (when wild_life-mode-electric-dot-full-predicate-template
 	(save-excursion
 	  (end-of-line)
 	  (insert ".\n"))))
     ;; Default
     (t
      (insert "\n"))
     )))

(defun wild_life-mode-electric--underscore ()
  "Replace variable with an underscore.
If `wild_life-mode-electric-underscore-flag' is non-nil and the point is
on a variable then replace the variable with underscore and skip
the following comma and whitespace, if any."
  (when wild_life-mode-electric-underscore-flag
    (let ((case-fold-search nil))
      (when (and (not (nth 8 (syntax-ppss)))
                 (eq (char-before) ?_)
                 (save-excursion
                   (skip-chars-backward "[:alpha:]_")
                   (looking-at "\\_<[_[:upper:]][[:alnum:]_]*\\_>")))
        (replace-match "_")
        (skip-chars-forward ", \t\n")))))

(defun wild_life-mode-post-self-insert ()
  (pcase last-command-event
    (`?_ (wild_life-mode-electric--underscore))
    (`?- (wild_life-mode-electric--dash))
    (`?: (wild_life-mode-electric--colon))
    ((or `?\( `?\; `?>) (wild_life-mode-electric--if-then-else))
    (`?. (wild_life-mode-electric--dot))))

(defun wild_life-mode-find-term (functor arity &optional prefix)
  "Go to the position at the start of the next occurrence of a term.
The term is specified with FUNCTOR and ARITY.  The optional argument
PREFIX is the prefix of the search regexp."
  (let* (;; If prefix is not set then use the default "\\<"
         (prefix (if (not prefix)
                     "\\<"
                   prefix))
         (regexp (concat prefix functor))
         (i 1))

    ;; Build regexp for the search if the arity is > 0
    (if (= arity 0)
        ;; Add that the functor must be at the end of a word. This
        ;; does not work if the arity is > 0 since the closing )
        ;; is not a word constituent.
        (setq regexp (concat regexp "\\>"))
      ;; Arity is > 0, add parens and commas
      (setq regexp (concat regexp "("))
      (while (< i arity)
        (setq regexp (concat regexp ".+,"))
        (setq i (1+ i)))
      (setq regexp (concat regexp ".+)")))

    ;; Search, and return position
    (if (re-search-forward regexp nil t)
        (goto-char (match-beginning 0))
      (error "Term not found"))
    ))

(defun wild_life-mode-variables-to-anonymous (beg end)
  "Replace all variables within a region BEG to END by anonymous variables."
  (interactive "r")
  (save-excursion
    (let ((case-fold-search nil))
      (goto-char end)
      (while (re-search-backward "\\<[A-Z_][a-zA-Z_0-9]*\\>" beg t)
        (progn
          (replace-match "_")
          (backward-char)))
      )))

;;(defun wild_life-mode-regexp-dash-continuous-chars (chars)
;;  (let ((ints (mapcar #'wild_life-mode-char-to-int (string-to-list chars)))
;;        (beg 0)
;;        (end 0))
;;    (if (null ints)
;;        chars
;;      (while (and (< (+ beg 1) (length chars))
;;                  (not (or (= (+ (nth beg ints) 1) (nth (+ beg 1) ints))
;;                           (= (nth beg ints) (nth (+ beg 1) ints)))))
;;        (setq beg (+ beg 1)))
;;      (setq beg (+ beg 1)
;;            end beg)
;;      (while (and (< (+ end 1) (length chars))
;;                  (or (= (+ (nth end ints) 1) (nth (+ end 1) ints))
;;                      (= (nth end ints) (nth (+ end 1) ints))))
;;        (setq end (+ end 1)))
;;      (if (equal (substring chars end) "")
;;          (substring chars 0 beg)
;;        (concat (substring chars 0 beg) "-"
;;                (wild_life-mode-regexp-dash-continuous-chars (substring chars end))))
;;    )))

;;(defun wild_life-mode-condense-character-sets (regexp)
;;  "Condense adjacent characters in character sets of REGEXP."
;;  (let ((next -1))
;;    (while (setq next (string-match "\\[\\(.*?\\)\\]" regexp (1+ next)))
;;      (setq regexp (replace-match (wild_life-mode-dash-letters (match-string 1 regexp))
;;				  t t regexp 1))))
;;  regexp)

;;-------------------------------------------------------------------
;; Menu stuff (both for the editing buffer and for the inferior
;; wild_life-mode buffer)
;;-------------------------------------------------------------------

(unless (fboundp 'region-exists-p)
  (defun region-exists-p ()
    "Non-nil if the mark is set.  Lobotomized version for Emacsen that do not provide their own."
    (mark)))


;; GNU Emacs ignores `easy-menu-add' so the order in which the menus
;; are defined _is_ important!

(easy-menu-define
  wild_life-mode-menu-help (list wild_life-mode-map wild_life-mode-inferior-mode-map)
  "Help menu for the Wild_Life-Mode mode."
  ;; FIXME: Does it really deserve a whole menu to itself?
  `(,(if (featurep 'xemacs) "Help"
       ;; Not sure it's worth the trouble.  --Stef
       ;; (add-to-list 'menu-bar-final-items
       ;;         (easy-menu-intern "Wild_Life-Mode-Help"))
       "Wild_Life-Mode-help")
    ["On predicate" wild_life-mode-help-on-predicate wild_life-mode-help-function-i]
    ["Apropos" wild_life-mode-help-apropos (eq wild_life-mode-system 'swi)]
    "---"
    ["Describe mode" describe-mode t]))

(easy-menu-define
  wild_life-mode-edit-menu-runtime wild_life-mode-map
  "Runtime Wild_Life-Mode commands available from the editing buffer"
  ;; FIXME: Don't use a whole menu for just "Run Mercury".  --Stef
  `("System"
    ;; Runtime menu name.
    ,@(unless (featurep 'xemacs)
        '(:label (cond ((eq wild_life-mode-system 'eclipse) "ECLiPSe")
                       ((eq wild_life-mode-system 'mercury) "Mercury")
                       (t "System"))))

    ;; Consult items, NIL for mercury.
    ["Consult file" wild_life-mode-consult-file
     :included (not (eq wild_life-mode-system 'mercury))]
    ["Consult buffer" wild_life-mode-consult-buffer
     :included (not (eq wild_life-mode-system 'mercury))]
    ["Consult region" wild_life-mode-consult-region :active (region-exists-p)
     :included (not (eq wild_life-mode-system 'mercury))]
    ["Consult predicate" wild_life-mode-consult-predicate
     :included (not (eq wild_life-mode-system 'mercury))]

    ;; Compile items, NIL for everything but SICSTUS.
    ,(if (featurep 'xemacs) "---"
       ["---" nil :included (eq wild_life-mode-system 'sicstus)])
    ["Compile file" wild_life-mode-compile-file
     :included (eq wild_life-mode-system 'sicstus)]
    ["Compile buffer" wild_life-mode-compile-buffer
     :included (eq wild_life-mode-system 'sicstus)]
    ["Compile region" wild_life-mode-compile-region :active (region-exists-p)
     :included (eq wild_life-mode-system 'sicstus)]
    ["Compile predicate" wild_life-mode-compile-predicate
     :included (eq wild_life-mode-system 'sicstus)]

    ;; Debug items, NIL for Mercury.
    ,(if (featurep 'xemacs) "---"
       ["---" nil :included (not (eq wild_life-mode-system 'mercury))])
    ;; FIXME: Could we use toggle or radio buttons?  --Stef
    ["Debug" wild_life-mode-debug-on :included (not (eq wild_life-mode-system 'mercury))]
    ["Debug off" wild_life-mode-debug-off
     ;; In SICStus, these are pairwise disjunctive,
     ;; so it's enough with a single "off"-command
     :included (not (memq wild_life-mode-system '(mercury sicstus)))]
    ["Trace" wild_life-mode-trace-on :included (not (eq wild_life-mode-system 'mercury))]
    ["Trace off" wild_life-mode-trace-off
     :included (not (memq wild_life-mode-system '(mercury sicstus)))]
    ["Zip" wild_life-mode-zip-on :included (and (eq wild_life-mode-system 'sicstus)
                                        (wild_life-mode-atleast-version '(3 . 7)))]
    ["All debug off" wild_life-mode-debug-off
     :included (eq wild_life-mode-system 'sicstus)]
    ["Source level debugging"
     wild_life-mode-toggle-sicstus-sd
     :included (and (eq wild_life-mode-system 'sicstus)
                    (wild_life-mode-atleast-version '(3 . 7)))
     :style toggle
     :selected wild_life-mode-use-sicstus-sd]

    "---"
    ["Run" run-wild_life-mode
     :suffix (cond ((eq wild_life-mode-system 'eclipse) "ECLiPSe")
                   ((eq wild_life-mode-system 'mercury) "Mercury")
                   (t "Wild_Life-Mode"))]))

(easy-menu-define
  wild_life-mode-edit-menu-insert-move wild_life-mode-map
  "Commands for Wild_Life-Mode code manipulation."
  '("Wild_Life-Mode"
    ["Comment region" comment-region (region-exists-p)]
    ["Uncomment region" wild_life-mode-uncomment-region (region-exists-p)]
    ["Add comment/move to comment" indent-for-comment t]
    ["Convert variables in region to '_'" wild_life-mode-variables-to-anonymous
     :active (region-exists-p) :included (not (eq wild_life-mode-system 'mercury))]
    "---"
    ["Insert predicate template" wild_life-mode-insert-predicate-template t]
    ["Insert next clause head" wild_life-mode-insert-next-clause t]
    ["Insert predicate spec" wild_life-mode-insert-predspec t]
    ["Insert module modeline" wild_life-mode-insert-module-modeline t]
    "---"
    ["Beginning of clause" wild_life-mode-beginning-of-clause t]
    ["End of clause" wild_life-mode-end-of-clause t]
    ["Beginning of predicate" wild_life-mode-beginning-of-predicate t]
    ["End of predicate" wild_life-mode-end-of-predicate t]
    "---"
    ["Indent line" indent-according-to-mode t]
    ["Indent region" indent-region (region-exists-p)]
    ["Indent predicate" wild_life-mode-indent-predicate t]
    ["Indent buffer" wild_life-mode-indent-buffer t]
    ["Align region" align (region-exists-p)]
    "---"
    ["Mark clause" wild_life-mode-mark-clause t]
    ["Mark predicate" wild_life-mode-mark-predicate t]
    ["Mark paragraph" mark-paragraph t]
    ;;"---"
    ;;["Fontify buffer" font-lock-fontify-buffer t]
    ))

(defun wild_life-mode-menu ()
  "Add the menus for the Wild_Life-Mode editing buffers."

  (easy-menu-add wild_life-mode-edit-menu-insert-move)
  (easy-menu-add wild_life-mode-edit-menu-runtime)

  ;; Add predicate index menu
  (setq-local imenu-create-index-function
              'imenu-default-create-index-function)
  ;;Milan (this has problems with object methods...)  ###### Does it? (Stefan)
  (setq-local imenu-prev-index-position-function
              #'wild_life-mode-beginning-of-predicate)
  (setq-local imenu-extract-index-name-function #'wild_life-mode-get-predspec)

  (if (and wild_life-mode-imenu-flag
           (< (count-lines (point-min) (point-max)) wild_life-mode-imenu-max-lines))
      (imenu-add-to-menubar "Predicates"))

  (easy-menu-add wild_life-mode-menu-help))

(easy-menu-define
  wild_life-mode-inferior-menu-all wild_life-mode-inferior-mode-map
  "Menu for the inferior Wild_Life-Mode buffer."
  `("Wild_Life-Mode"
    ;; Runtime menu name.
    ,@(unless (featurep 'xemacs)
        '(:label (cond ((eq wild_life-mode-system 'eclipse) "ECLiPSe")
                       ((eq wild_life-mode-system 'mercury) "Mercury")
                       (t "Wild_Life-Mode"))))

    ;; Debug items, NIL for Mercury.
    ,(if (featurep 'xemacs) "---"
       ["---" nil :included (not (eq wild_life-mode-system 'mercury))])
    ;; FIXME: Could we use toggle or radio buttons?  --Stef
    ["Debug" wild_life-mode-debug-on :included (not (eq wild_life-mode-system 'mercury))]
    ["Debug off" wild_life-mode-debug-off
     ;; In SICStus, these are pairwise disjunctive,
     ;; so it's enough with a single "off"-command
     :included (not (memq wild_life-mode-system '(mercury sicstus)))]
    ["Trace" wild_life-mode-trace-on :included (not (eq wild_life-mode-system 'mercury))]
    ["Trace off" wild_life-mode-trace-off
     :included (not (memq wild_life-mode-system '(mercury sicstus)))]
    ["Zip" wild_life-mode-zip-on :included (and (eq wild_life-mode-system 'sicstus)
                                        (wild_life-mode-atleast-version '(3 . 7)))]
    ["All debug off" wild_life-mode-debug-off
     :included (eq wild_life-mode-system 'sicstus)]
    ["Source level debugging"
     wild_life-mode-toggle-sicstus-sd
     :included (and (eq wild_life-mode-system 'sicstus)
                    (wild_life-mode-atleast-version '(3 . 7)))
     :style toggle
     :selected wild_life-mode-use-sicstus-sd]

    ;; Runtime.
    "---"
    ["Interrupt Wild_Life-Mode" comint-interrupt-subjob t]
    ["Quit Wild_Life-Mode" comint-quit-subjob t]
    ["Kill Wild_Life-Mode" comint-kill-subjob t]))


(defun wild_life-mode-inferior-menu ()
  "Create the menus for the Wild_Life-Mode inferior buffer.
This menu is dynamically created because one may change systems during
the life of an Emacs session."
  (easy-menu-add wild_life-mode-inferior-menu-all)
  (easy-menu-add wild_life-mode-menu-help))

(defun wild_life-mode-version ()
  "Echo the current version of Wild_Life-Mode mode in the minibuffer."
  (interactive)
  (message "Using Wild_Life-Mode mode version %s" wild_life-mode-version))

(provide 'wild_life-mode)

;;; wild_life-mode.el ends here
