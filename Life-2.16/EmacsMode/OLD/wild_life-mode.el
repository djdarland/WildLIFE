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


(define-abbrev-table 'wild_life-mode-abbrev-table ())


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
  (setq parse-sexp-ignore-comments nil))



(defvar wild_life-mode-map (make-sparse-keymap)
  "Keymap used in Life mode.")

(define-key wild_life-mode-map "\t" 'life-indent-command)
(define-key wild_life-mode-map "\e\C-\\" 'indent-region)
(define-key wild_life-mode-map "\C-x%" 'life-comment-region)
(define-key wild_life-mode-map "\C-x$" 'life-uncomment-region)
(define-key wild_life-mode-map "\C-j" 'reindent-then-newline-and-indent)
(define-key wild_life-mode-map "\e\C-q" 'indent-life-exp)



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Life mode
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;###autoload
(defun wild_life-mode ()
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
  (run-hooks 'wild_life-mode-hook))


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








