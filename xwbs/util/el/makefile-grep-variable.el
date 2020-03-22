#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to grep variable from generated makefile
;; @author
;; + 隐星魂 (Roy.Sun) <www.starsoul.tech>
;; @copyright
;; + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
;; > Licensed under the Apache License, Version 2.0 (the "License");
;; > you may not use this file except in compliance with the License.
;; > You may obtain a copy of the License at
;; >
;; >         http://www.apache.org/licenses/LICENSE-2.0
;; >
;; > Unless required by applicable law or agreed to in writing, software
;; > distributed under the License is distributed on an "AS IS" BASIS,
;; > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; > See the License for the specific language governing permissions and
;; > limitations under the License.

(defvar elscript (expand-file-name (nth 2 command-line-args)) "script name")
(defvar elpath (file-name-directory elscript))
(push elpath load-path)
(require 'errno)

;;;;;;;; ;;;;;;;; ;;;;;;;; config ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defconst cfg-log-lv 2)

;;;;;;;; ;;;;;;;; ;;;;;;;; log function ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defconst DBG "[DBG]")
(defconst DBG-LV 0)
(defconst INF "[INF]")
(defconst INF-LV 1)
(defconst WRN "[WRN]")
(defconst WRN-LV 2)
(defconst ERR "[ERR]")
(defconst ERR-LV 3)

(defun logd (fmtstr &rest arglist)
  (if (<= cfg-log-lv DBG-LV)
      (princ (apply 'format (concat DBG " " fmtstr "\n") arglist))))
(defun logi (fmtstr &rest arglist)
  (if (<= cfg-log-lv INF-LV)
      (princ (apply 'format (concat INF " " fmtstr "\n") arglist))))
(defun logw (fmtstr &rest arglist)
  (if (<= cfg-log-lv WRN-LV)
      (princ (apply 'format (concat WRN " " fmtstr "\n") arglist))))
(defun loge (fmtstr &rest arglist)
  (if (<= cfg-log-lv ERR-LV)
      (princ (apply 'format (concat ERR " " fmtstr "\n") arglist))))
(defmacro logtag (tag)
  (list 'car (list 'read-from-string (list 'concat "log" (list 'format "%s" tag)))))

(defun log (tag fmtstr &rest arglist)
  (apply (logtag tag) fmtstr arglist))

;;;;;;;; ;;;;;;;; ;;;;;;;; arguments ;;;;;;;; ;;;;;;;; ;;;;;;;;
(setq debug-on-error t)
(setq make-backup-files nil)
(defvar argc (length argv) "Count of arguments")
(defvar in nil "Input makefile")
(defvar variable nil "which variable to be grepped")
(log 'd "%s" argv)
(let ((options-done nil))
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      (cond
       (options-done (setq in option))

       ;; options after "--"
       ((string= option "--") (setq options-done t))

       ;; --variable foo
       ;; -a foo
       ;; --variable=foo
       ((string= option "--variable") (setq variable (pop argv)))
       ((string= option "-a") (setq variable (pop argv)))
       ((string-match "\\`--variable=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq variable (match-string 1 option)))

       ;; Unknown options
       ((string-prefix-p "--" option)
        (log 'e "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ((string-prefix-p "-" option)
        (log 'e "Unknown option: %s" option)
        (kill-emacs EINVAL))

       (t (setq in option)))))

  (log 'd "in:%s" in)
  (log 'd "variable:%s" variable)

  (unless (> (length variable) 0)
    (log 'e "Missing argument for --variable!")
    (kill-emacs EINVAL)))

;;;;;;;; ;;;;;;;; ;;;;;;;; main script ;;;;;;;; ;;;;;;;; ;;;;;;;;
;; read to input buffer
(if (or (null in) (string= "-" in))
    (progn
      (setq in-buffer (get-buffer-create "input"))
      (with-current-buffer in-buffer
        (progn
          (set-buffer-multibyte t)
          (condition-case nil
            (let (line)
              (while (setq line (read-from-minibuffer ""))
                (insert line)
                (insert "\n")))
            (error nil)))))
    (progn
      (setq in-buffer (find-file-noselect in))))

;; Iterate over the input buffer to remove all '\r'
(set-buffer in-buffer)
(set-buffer-multibyte t)
(goto-char (point-min))
(delete-trailing-whitespace)
(catch 'break
  (let ((iterpoint 1))
    (goto-char (point-min))
    (while (< iterpoint (point-max))
      (setq iterpoint (re-search-forward "\r\n" nil t))
      (if (null iterpoint)
          (throw 'break nil)
          (replace-match "\n")))))

(let ((iterpoint 1)
      (variable-beginning 1)
      (variable-end 1)
      (variable-content ""))
  (set-buffer in-buffer)
  (set-buffer-multibyte t)
  (goto-char (point-min))
  (setq iterpoint (re-search-forward (concat variable "[ :?]*=\\(.+\\)\\\\$") nil t))
  (if (null iterpoint)
      (progn
        (setq iterpoint (re-search-forward (concat variable "[ :?]*=\\(.+\\)$") nil t))
        (if (null iterpoint)
            (setq variable-content "")
            (progn
              (setq variable-beginning (match-beginning 0))
              (setq variable-end (match-end 0))
              (setq variable-content (match-string 1)))))
      (progn
        (setq variable-beginning (match-beginning 0))
        (setq variable-content (match-string 1))
        (setq iterpoint (line-beginning-position))
        (log 'd "%d:%d:%d" variable-beginning iterpoint (point-max))
        (let ((done nil)
              line-beginning
              line-end
              line-content)
          (while (and (< iterpoint (point-max)) (null done))
            (setq line-beginning (line-beginning-position 2))
            (setq line-end (line-end-position 2))
            (setq line-content (buffer-substring line-beginning line-end))
            (if (string-match-p ".+\\\\" line-content)
                (progn
                  (setq line-content
                        (replace-regexp-in-string
                         "\\(.+\\)\\(\\\\\\)" "" line-content
                         nil nil 2))
                  (setq done nil))
                (progn
                  (setq done t)))
            (log 'd "line <%d:%d>%s" line-beginning line-end line-content)
            (setq variable-content (concat variable-content " " line-content))
            (setq variable-end line-end)
            (setq iterpoint line-beginning)
            (goto-char iterpoint)))))
  (log 'd "variable <%d:%d>%s"
        variable-beginning
        variable-end
        variable-content)
  (princ (concat variable-content "\n")))

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
