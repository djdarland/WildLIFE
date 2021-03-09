;; wild_life-mode.el --- major mode for editing Life under Emacs

;; Author: Bruno Dumant <dumant@prl.dec.com>
;; Keywords: languages

;; This package provides a major mode for editing Life.  It knows
;; about Life syntax and comments.

;;; Code:
;; Modified 5/12/20124 Dennis J Darland

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Variables 
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
 (cond (window-system
           (setq hilit-mode-enable-list  '(not text-mode)
                 hilit-background-mode   'light
                 hilit-inhibit-hooks     nil
                 hilit-inhibit-rebinding nil)
           (require 'hilit19)))
(setq wild_life-emphasize-flag nil)
(setq hilit-auto-highlight t)
(setq hilit-quietly t)
;; Default faces and values (ok with a dark background)


(defvar wild_life-mode-syntax-table nil)
(defvar wild_life-mode-abbrev-table nil)
(defvar wild_life-mode-map (make-sparse-keymap)
  "Keymap used in wild_life-mode.")

(defvar life-brace-offset 4)
(defvar life-multiline-offset 2)
(defvar close-position 0)
(defvar small-comments nil)
(defvar life-tab-always-indent t)
(defvar clause-beginning 0)
(defvar sentence-start-col (* 2 life-brace-offset))
(defvar sentence-start-pos 0)
(defvar open-pile nil)

(setq max-lisp-eval-depth 500)

(if (not wild_life-mode-syntax-table)
    (let ( (table (make-syntax-table)) )
      (modify-syntax-entry ?_ "w" table)
      (modify-syntax-entry ?\\ "\\" table)
      (modify-syntax-entry ?/ "." table)
      (modify-syntax-entry ?* "." table)
      (modify-syntax-entry ?+ "." table)
      (modify-syntax-entry ?- "." table)
      (modify-syntax-entry ?= "." table)
      (modify-syntax-entry ?% "<" table)
      (modify-syntax-entry ?\n ">" table)
      (modify-syntax-entry ?< "." table)
      (modify-syntax-entry ?> "." table)
      (modify-syntax-entry ?\' "\"" table)
      (setq wild_life-mode-syntax-table table)))

;;;;;;;;;;; emphasize follows

(defun wild_life-emphasize-line (incr end)
  (if wild_life-emphasize-flag
      (let ((context (cons 0 nil)) start final-point case
	    (test (- (point) clause-beginning))
	    begin unhilight-start)
	
	(if (or (> test 100) (< test 0))
	    (setq clause-beginning (beginning-of-life-clause-pos)))
	(setq case case-fold-search)
	(setq case-fold-search nil)
	(if incr
	    (progn
	      (end-of-line)
	      (setq end (point))))
	(beginning-of-line)
	(setq begin (point))
	
	;; unhilighting region
	(setq unhilight-start begin)
	(while (< unhilight-start end)
	  (mapcar (function (lambda (ovr)
			      (and (overlay-get ovr 'hilit)
				   (delete-overlay ovr))))
		  (overlays-at unhilight-start))
	  (setq unhilight-start (next-overlay-change unhilight-start)))

	;; highlighting
	(if (inside-string incr)
	    (progn
	      (skip-chars-forward " \t")
	      (setq start (point))
	      (goto-string-end incr)
	      (hilit-region-set-face start (point) 'life-value)))
	(while (not (or (eobp) (>= (point) end)))
	  (skip-chars-forward " \t")
	  (setq start (point))
	  (cond ((looking-at "[a-z]")
		 (goto-atom-end)
		 (hilit-region-set-face start (point) 'life-atom)
		 )
		((looking-at "[A-Z_]")
		 (goto-atom-end)
		 (hilit-region-set-face start (point) 'life-variable))
		((looking-at "[`~#$^&*+=\\/:<>-]")
		 (goto-op-end)
		 (hilit-region-set-face start (point) 'life-operator))
		((looking-at "\"")
		 (forward-char 1)
		 (goto-string-end incr) 
		 (hilit-region-set-face start (point) 'life-value)
		 )
		((looking-at "%")
		 (end-of-line)
		 (hilit-region-set-face start (point) 'life-comment))
		((looking-at "[0-9]")
		 (goto-number-end)
		 (hilit-region-set-face start (point) 'life-value)
		 )
		((looking-at "'")
		 (goto-quoted-atom-end)
		 (hilit-region-set-face start (point) 'life-atom)
		 )
		((looking-at "[;?,]")
		 (forward-char 1)
		 (hilit-region-set-face start (point) 'life-syntax)
		 )
		((looking-at "[{(]")
		 (forward-char 1)
		 (setq context (cons 0 context))
		 (hilit-region-set-face start (point) 'life-syntax)
		 )
		((looking-at "[})]")
		 (forward-char 1)
		 (if (not (equal (cdr context) nil))
		     (setq context (cdr context)))
		 (hilit-region-set-face start (point) 'life-syntax)
		 )
		((looking-at "[.]")
		 (forward-char 1)
		 (if (or (looking-at "[ \t\n%]") (eobp))
		     (setq final-point t)
		   (setq final-point nil))
		 (if final-point
		     (hilit-region-set-face start (point) 'life-syntax)
		   (hilit-region-set-face start (point) 'life-operator)))
		((looking-at "[[]")
		 (forward-char 1)
		 (setq context (cons 1 context))
		 (hilit-region-set-face start (point) 'life-syntax)
		 )
		((looking-at "[]]")
		 (forward-char 1)
		 (if (not (equal (cdr context) nil))
		     (setq context (cdr context)))
		 (hilit-region-set-face start (point) 'life-syntax))
		((looking-at "!")
		 (forward-char 1)
		 (hilit-region-set-face start (point) 'life-operator))
		((looking-at "|[`|.#$^&*+=\\/:<>-]")
		 (goto-op-end)
		 (hilit-region-set-face start (point) 'life-operator))
		((looking-at "|")
		 (forward-char 1)
		 (if (= (car context) 1)
		     (progn
		       (hilit-region-set-face start (point) 'life-syntax))
		   (hilit-region-set-face start (point) 'life-operator)))
		((eobp) t)
		(t (forward-char 1))))
	)))


(defun goto-atom-end ()
  (forward-char 1)
  (looking-at "[A-Za-z_0-9]*")
  (goto-char (match-end 0)))


(defun goto-quoted-atom-end ()
  (forward-char 1)
  (if (looking-at "[^']*'")
      (goto-char (match-end 0))))


(defun goto-string-end (incr)
  (if incr
      (looking-at "[^\"\n]*")
      (looking-at "[^\"]*"))
  (goto-char (match-end 0))
  (if (looking-at "\"")
      (forward-char 1))
  )

(defun goto-op-end ()
  (forward-char 1)
  (looking-at "[`~#$^&*+=\\/:<>.|-]*")
  (goto-char (match-end 0)))

(defun goto-number-end ()
  (looking-at "[0-9]*")
  (goto-char (match-end 0))
  (cond	((looking-at "[.]")
	 (forward-char 1)
	 (if (looking-at "[0-9]+")
	     (goto-char (match-end 0))
	   (forward-char -1)))
	(t t)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; emphasize a region or a buffer
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(defun wild_life-emphasize-region (begin end &optional quietly)
  "Emphasize region of life code."
  (interactive "r")
  (or quietly hilit-quietly
      (message "Highlighting region..."))
  (save-excursion
    (goto-char end)
    (setq end (point-marker))
    (goto-char begin)
    (wild_life-emphasize-line nil end)
    (move-marker end nil))
  (setq mark-active nil)
  (run-hooks 'deactivate-mark-hook)
  (or quietly hilit-quietly
      (message "Done.")))

(defun wild_life-emphasize-buffer ()
   "Emphasize the buffer."
   (interactive)
   (if mark-active
       (wild_life-emphasize-region (min (point) (mark))
			    (max (point) (mark)))
     (if wild_life-emphasize-flag
	 (wild_life-emphasize-region (point-min) (point-max)))))


(defun life-repaint-command (arg)
  "Rehighlights according to the value of hilit-auto-rehighlight, or the
prefix argument if that is specified.
\t\\[life-repaint-command]\t\trepaint according to hilit-auto-rehighlight
\t^U \\[life-repaint-command]\trepaint entire buffer
\t^U - \\[life-repaint-command]\trepaint visible portion of buffer
\t^U n \\[life-repaint-command]\trepaint n lines to either side of point"
  (interactive "P") 
  (let (st en quietly)
    (or arg (setq arg hilit-auto-rehighlight))
    (cond ((or (eq  arg 'visible) (eq arg '-))
	   (setq st (window-start) en (window-end) quietly t))
	  ((numberp arg)
	   (setq st (save-excursion (forward-line (- arg)) (point))
		 en (save-excursion (forward-line arg) (point))))
	  (arg
	   (wild_life-emphasize-buffer)))
    (if st
	  (wild_life-emphasize-region st en quietly))))

(defun life-recenter (arg)
  "Recenter, then rehighlight according to hilit-auto-rehighlight.  If called
with an unspecified prefix argument (^U but no number), then a rehighlight of
the entire buffer is forced."
  (interactive "P")
  (recenter arg)
  ;; force display update
  (sit-for 0)
  (life-repaint-command (consp arg)))

(defun life-yank (arg)
  "Yank with rehighlighting"
  (interactive "*P")
  (let ((transient-mark-mode nil))
    (yank arg)
    (and hilit-auto-rehighlight
	 (wild_life-emphasize-region (region-beginning) (region-end) t))
    (setq this-command 'yank)))

(defun life-yank-pop (arg)
  "Yank-pop with rehighlighting"
  (interactive "*p")
  (let ((transient-mark-mode nil))
    (yank-pop arg)
    (and hilit-auto-rehighlight
	 (wild_life-emphasize-region (region-beginning) (region-end) t))
    (setq this-command 'yank)))



;;;;;;;;;; end emphasize


(defun life-post-command-hook ()
  (if (and (eq this-command 'self-insert-command)
;;	       (eq this-command 'backward-delete-char)
;;	       (eq this-command 'backward-delete-char-untabify))
	   (> (point) 1))
      (save-excursion
	(wild_life-emphasize-line t 0 )
	)
    ))


;;;;;;;;;;;;; end emphasize

(defun wild_life-mode-variables ()
  (set-syntax-table wild_life-mode-syntax-table)
  (setq local-abbrev-table wild_life-mode-abbrev-table)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "^%%%\\|^$\\|" page-delimiter)) ;'%%%..'
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'life-indent-line-2)
  (make-local-variable 'comment-start)
  (setq comment-start "%")
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "%+ *")
  (make-local-variable 'comment-column)
  (setq comment-column 48)
  (make-local-variable 'comment-indent-hook)
  (setq comment-indent-hook 'life-comment-indent)
  (make-local-variable 'require-final-newline)
  (setq require-final-newline t)
  (make-local-variable 'comment-end)
  (setq comment-end "")
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments nil)
;;;;;;;; begin emphasize
(define-abbrev-table 'wild_life-mode-abbrev-table ())
(make-face 'life-comment)
(set-face-foreground 'life-comment "cyan")

(make-face 'life-keyword)
(set-face-foreground 'life-keyword "springgreen")

(make-face 'life-atom)
(set-face-foreground 'life-atom "white")

(make-face 'life-operator)
(set-face-foreground 'life-operator "orange")

(make-face 'life-value)
(set-face-foreground 'life-value "yellow")

(make-face 'life-syntax)
(set-face-foreground 'life-syntax "white")

(make-face 'life-variable)
(set-face-foreground 'life-variable "moccasin")



)



(defvar wild_life-mode-map (make-sparse-keymap)
  "Keymap used in Life mode.")

(define-key wild_life-mode-map "\t" 'life-indent-command)
(define-key wild_life-mode-map "\e\C-\\" 'indent-region)
(define-key wild_life-mode-map "\C-x%" 'life-comment-region)
(define-key wild_life-mode-map "\C-x$" 'life-uncomment-region)
(define-key wild_life-mode-map "\C-j" 'reindent-then-newline-and-indent)
(define-key wild_life-mode-map "\e\C-q" 'indent-life-exp)
;;; emphasize follows
(define-key wild_life-mode-map  "\C-c\C-b" 'wild_life-emphasize-buffer)
(define-key wild_life-mode-map  "\C-y" 'life-yank)
(define-key wild_life-mode-map  "\C-l" 'life-recenter)
(define-key wild_life-mode-map [?\C-\S-l] 'life-repaint-command)
;;; end emphasize

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Life mode
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;###autoload
(defun wl-mode ()
  "Major mode for editing Life code. 
Tab at left margin indents for Life code (see life-tab-always-indent below),
Blank lines and `%%%...' separate paragraphs.  `%'s start comments.
Commands:
\\{wild_life-mode-map}

 There are three kinds of comments:
    - large comments start with %%%. They are always indented at column 0.
    - mid-comments start with %%. They are indented as life code, unless there
      is a comment on the line before. In the latter case, the comments are 
      aligned.
    - small comments start with %.
      If small-comments is non-nil, they are idented to comment-column 
      (default value is 48), unless there is a comment on the line before.
      Otherwise, they are idented like mid-comments.
      Default of small-comments is nil.
 
 life-tab-always-indent
    Non-nil means TAB in Life mode should always reindent the current line,
    regardless of where in the line point is when the TAB command is used.
    Default is t.
 life-brace-offset
    Extra indentation for line if it starts with an open brace.
    Default is 4.

 life-multiline-offset 
   It is the amount of extra indentation for more than one line long sentences
   (sequence of terms ending with , ; or |). Its default value is 2. 

Entry to this mode calls the value of `wild_life-mode-hook'
if that value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (use-local-map wild_life-mode-map)
  (setq major-mode 'wild_life-mode)
  (setq mode-name "Life")
  (wild_life-mode-variables)
;;;;;;;;;;;; emphasize follows
  (if hilit-auto-highlight
      (progn
	(if (> buffer-saved-size (car hilit-auto-rehighlight-fallback))
	    (setq hilit-auto-rehighlight
		  (cdr hilit-auto-rehighlight-fallback)))
	(if (> buffer-saved-size hilit-auto-highlight-maxout) nil
	  (wild_life-emphasize-buffer))))
  (make-variable-buffer-local 'post-command-hook)
  (setq post-command-hook 'life-post-command-hook)



;;;;;;;;;;; end emphasize
  (run-hooks 'wild_life-mode-hook)

)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Indentation command
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun life-indent-command (&optional whole-exp)
  (interactive "P")
  "Indent current line as Life code, or in some cases insert a tab character.
If life-tab-always-indent is non-nil (the default), always indent current line.
Otherwise, indent the current line only if point is at the left margin
or in the line's indentation; otherwise insert a tab.

A numeric argument, regardless of its value,
means indent rigidly all the lines of the expression starting after point
so that this line becomes properly indented.
The relative indentation among the lines of the expression are preserved."
  (if whole-exp
      ;; If arg, always indent this line as Life
      ;; and shift remaining lines of expression the same amount.
      (let ((shift-amt (life-indent-line))
	    beg end)
	(save-excursion
	  (if life-tab-always-indent
	      (beginning-of-line))
	  (setq beg (point))
	  (forward-sexp 1)
	  (setq end (point))
	  (goto-char beg)
	  (forward-line 1)
	  (setq beg (point)))
	(if (> end beg)
	    (indent-code-rigidly beg end shift-amt "#")))
    (if (and (not life-tab-always-indent)
	     (save-excursion
	       (skip-chars-backward " \t")
	       (not (bolp))))
	(insert-tab)
      (life-indent-line))))


;; indenting code

(defun life-indent-line ()
  "Indent current line as Life code."
  (interactive "p")
  (beginning-of-line)
  (life-real-indent)
  (if (looking-at "[ \t]*\n")
      (skip-chars-forward " \t")
    (end-of-line)
    (skip-chars-backward " \t")))

(defun life-indent-line-2 ()
  "Indent current line as Life code."
  (interactive "p")
  (beginning-of-line)
  (life-real-indent)
  (let ((pos (- (point-max) (point))))
    (if (> (- (point-max) pos) (point))
	(goto-char (- (point-max) pos)))))



(defun life-real-indent ()
  (setq clause-beginning (beginning-of-life-clause-pos))
  (if (not (inside-string t))
      (let ((indent (life-indent-level)) beg)
	(setq beg (point))
	(skip-chars-forward " \t")
	(if (zerop (- indent (current-column)))
	    nil
	  (delete-region beg (point))
	  (indent-to indent)))))



(defun life-indent-level ()
  "Return appropriate indentation for current line as Life code.
In usual case returns an integer: the column to indent to."
  (save-excursion
    (beginning-of-line)
    (skip-chars-forward " \t")
    (cond
     ((>= (- clause-beginning (point)) 0) 
      (setq sentence-start-pos 0)
      0)   ; Beginning of clause
     ((looking-at "%") (life-comment-indent))  ; comment
     (t
      (life-indent-level2 (point))))))

(defun life-indent-level2 (current-pos)
  (goto-char clause-beginning)
  (init-indent)
  (life-indent-level3 current-pos))

  
(defun life-indent-level3 (current-pos)
  "Return appropriate indentation for current line as Life code when it depends on the previous line."
  (while (< (point) current-pos)
    (looking-at "[^]})\"'{(%,;|.?\n[]*" )
    (goto-char (match-end 0))
    (if (< (point) current-pos)
	(cond ((looking-at "[{([]")
	       (save-context))
	      ((looking-at "[]})]")
	       (restore-context))
	      ((looking-at "[\"']")
	       (skip-string-forward))
	      ((looking-at "%")
	       (end-of-line))
	      ((looking-at "\n")
	       (forward-char 1)
	       (if (zerop sentence-start-pos)
		   (progn
		     (skip-void-comments)
		     (setq sentence-start-pos (point)))))
	      ((looking-at "[,;|]")
	       (forward-char 1)
	       (skip-void-comments)
	       (setq sentence-start-pos (min current-pos (point))))
	      ((looking-at "[.?]")
	       (forward-char 1)
	       (if (looking-at "[ \t\n]")
		   (progn
		     (skip-void-comments)
		     (setq clause-beginning (point))
		     (init-indent)))))))
  (goto-char current-pos)
  (cond ((>= (- clause-beginning (point)) 0) 
	 (setq sentence-start-pos 0)
	 0)
	((looking-at "[]});,|]")
	 (if open-pile
	     (car open-pile)
	   life-brace-offset))
	((zerop sentence-start-pos)
	 sentence-start-col)
	((> (point) sentence-start-pos)  
	 (+ life-multiline-offset sentence-start-col))
	(t
	 sentence-start-col)))

;; indenting comments

(defun life-comment-indent ()
  "Compute life comment indentation."
  (save-excursion
    (cond ((looking-at "%%%") 0)                   ; Large comment starts 
	  (t                                       ; Other comments
	   (let (col)
	     (setq col (current-column))
	     (forward-line -1)
	     (beginning-of-line)
	     (if (looking-at "[^%\n]*%")           ; if there are comments on the
					; line before, align. 
		 (progn
		   (goto-char (- (match-end 0) 1))
		   (current-column))
	       (forward-line 1)
	       (skip-chars-forward " \t\n")
	       (if (or (looking-at "%%") (not small-comments))
		   (life-indent-level2 (point))
		 comment-column)))))))



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Utilities used for indenting
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; go to the beginning of the current clause

(defun beginning-of-clause ()
  (interactive)
  (cond ((bobp)
	 t)
	((re-search-backward "^[^%\n]*[?.][ \t]*\\(%.*\n?\\)?\n" 
			     (bobp) t)
	 (goto-char (match-end 0)))
	(t
	 (beginning-of-buffer)))
  (skip-void-comments))


(defun beginning-of-life-clause-pos ()
  (save-excursion
    (beginning-of-clause)
    (point)))



;; initialization

(defun init-indent ()
  (setq sentence-start-pos 0) 
  (setq sentence-start-col (* 2 life-brace-offset))
  (setq open-pile nil))


;; Dealing with contexts

(defun save-context ()
  (let (indent-after
	(cc (current-column)) )
    (forward-char 1)
    (skip-void-comments-same-line)
    (if (looking-at "\n")
	(setq indent-after (+ life-brace-offset cc))
      (setq indent-after (current-column)))
    (skip-void-comments)
    (setq open-pile (list cc indent-after sentence-start-pos 
			  sentence-start-col open-pile))
    (setq sentence-start-pos (point))
    (setq sentence-start-col indent-after)
    ))


(defun restore-context ()
  (let (context)
    (if open-pile
	(progn
	  (setq sentence-start-pos (nth 2 open-pile))
	  (setq sentence-start-col (nth 3 open-pile))
	  (setq open-pile (nth 4 open-pile)))))
  (forward-char 1))


;; checking whether current position is inside a string or a quoted atom

(defun inside-string (incr)
  (if (or (bobp) (not incr))
      nil
    (in-string (point))))

(defun in-string (end-point)
  (save-excursion
    (goto-char clause-beginning)
    (in-string2 end-point)))

(defun in-string2 (end-point)
  (looking-at "[^\"'%]*")
  (goto-char (match-end 0))
  (if (>= (point) end-point)
      nil
    (cond ((looking-at "[\"']")
	   (skip-string-forward)
	   (if (>= (point) end-point)
	       t
	     (in-string2 end-point)))
	  ((looking-at "%")
	   (end-of-line)
	   (cond ((>= (point) end-point)
		  nil)
		 ((eobp)
		  nil)
		 (t
		  (forward-char 1)
		  (in-string2 end-point))))
	  ((eobp)
	   nil))))


;; skipping strings and quoted atoms

(defun skip-string-forward ()
  "skip strings or atoms forward"
  (while (looking-at "['\"]")
    (cond ((looking-at "\"[^\"]*\"?")
	   (goto-char (match-end 0)))
	  ((looking-at "'[^']*'?")
	   (goto-char (match-end 0))))))

(defun skip-void-comments ()
  (skip-chars-forward " \t\n")
  (while (looking-at "%.*\n?")
    (goto-char (match-end 0))
    (skip-chars-forward " \t\n")))

(defun skip-void-comments-same-line ()
  (looking-at "[ \t]*\\(%.*\\)*")
  (goto-char (match-end 0)))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Commenting regions
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		      

(defun life-comment-region (start end)
  (interactive "r")
  (comment-region start end 3))

(defun life-uncomment-region (start end)
  (interactive "r")
  (comment-region start end -3))



;;; wild_life-mode.el ends here








;; Life code editing commands for Emacs19
;; Bruno Dumant <dumant@prl.dec.com>

;; For hilighting Life Code
;; valid only for Emacs19

;; (if (emacs19-used)
;;    (cond (window-system ;;; make sure we're not a dumb tty
;;	   (setq hilit-mode-enable-list  '(not text-mode)
;;		 hilit-background-mode   'dark) ;;; or 'light or 'mono
;;	   
;;	   (require 'hilit19)
;;	   )))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Emphasize a line: this code is a life tokenizer
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;



;;; All the following is for better compatibility with hilit19.el
;;; Code was merely adapted from hilit19.el


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; HOOKS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Initialization.  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;; (defun wild_life-emphasize-mode () 

;;)
