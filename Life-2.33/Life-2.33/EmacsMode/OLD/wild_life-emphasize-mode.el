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

(setq wild_life-emphasize-flag t)

;; Default faces and values (ok with a dark background)

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


(defun life-post-command-hook ()
  (if (and (eq this-command 'self-insert-command)
;;	       (eq this-command 'backward-delete-char)
;;	       (eq this-command 'backward-delete-char-untabify))
	   (> (point) 1))
      (save-excursion
	(wild_life-emphasize-line t 0 )
	)
    ))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Emphasize a line: this code is a life tokenizer
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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



;;; All the following is for better compatibility with hilit19.el
;;; Code was merely adapted from hilit19.el


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; HOOKS
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Initialization.  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(defun wild_life-emphasize-mode () 
  (define-key life-mode-map  "\C-c\C-b" 'wild_life-emphasize-buffer)
  (and (not hilit-inhibit-rebinding)
       window-system
       (define-key life-mode-map  "\C-y" 'life-yank)
       (define-key life-mode-map  "\C-l" 'life-recenter))
  (define-key life-mode-map [?\C-\S-l] 'life-repaint-command)
  (if hilit-auto-highlight
      (progn
	(if (> buffer-saved-size (car hilit-auto-rehighlight-fallback))
	    (setq hilit-auto-rehighlight
		  (cdr hilit-auto-rehighlight-fallback)))
	(if (> buffer-saved-size hilit-auto-highlight-maxout) nil
	  (wild_life-emphasize-buffer))))
  (make-variable-buffer-local 'post-command-hook)
  (setq post-command-hook 'life-post-command-hook))
