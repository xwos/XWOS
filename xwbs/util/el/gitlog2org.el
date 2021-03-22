#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to convert git commit log to Org-mode document
;; @author
;; + 隐星魂 (Roy.Sun) <https://xwos.tech>
;; @copyright
;; + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

(defvar elscript (expand-file-name (nth 2 command-line-args)) "script full path")
(defvar elname (file-name-nondirectory elscript))
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
(defvar opt-in nil "Input file")
(defvar opt-out nil "Output file")
(logi "%s %s" elscript argv)
(let ((options-done nil))
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      (cond
       (options-done (setq opt-in option))

       ;; options after "--"
       ((string= option "--") (setq options-done t))

       ;; --out foo
       ;; -o foo
       ;; --out=foo
       ((string= option "--out") (setq opt-out (pop argv)))
       ((string= option "-o") (setq opt-out (pop argv)))
       ((string-match "\\`--out=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq opt-out (match-string 1 option)))

       ;; Unknown options
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       (t (setq opt-in option)))))

  (logi "opt-in:%s" opt-in)
  (logi "opt-out:%s" opt-out)

  (unless (> (length opt-out) 0)
    (loge "Missing argument for --out!")
    (kill-emacs EINVAL)))

;;;;;;;; ;;;;;;;; ;;;;;;;; main script ;;;;;;;; ;;;;;;;; ;;;;;;;;
;; clear output buffer
(setq out-buffer (find-file-noselect (concat opt-out ".org")))
(set-buffer out-buffer)
(erase-buffer)
(insert "#+STARTUP: showall\n")
(insert "#+STARTUP: hidestars\n")
(insert "#+TITLE: Changelog\n")
(insert "#+LANGUAGE: zh-CN\n")
(insert "#+OPTIONS: ^:{}\n")
(insert "#+OPTIONS: title:nil\n")
(insert "#+OPTIONS: toc:t\n\n")

;; read to input buffer
(if (or (null opt-in) (string= "-" opt-in))
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
      (setq in-buffer (find-file-noselect opt-in))))

;; delete blank-lines & trailing-whitespace
(set-buffer in-buffer)
(set-buffer-multibyte t)
(goto-char (point-min))
(delete-trailing-whitespace)
(let ((iterpoint 1))
  (while (< iterpoint (point-max))
    (setq iterpoint (re-search-forward "^$" nil t))
    (forward-line -1)
    (delete-blank-lines)))

;; Iterate over the input buffer
(catch 'break
  (let ((iterpoint 1)
        commit-beginning
        commit-end
        commit-id
        commit-author
        commit-date
        change-id
        commit-log-beginning
        commit-log
        commit-type
        commit-type-match)
    (set-buffer in-buffer)
    (goto-char (point-min))
    (while (< iterpoint (point-max))
      (goto-char iterpoint)

      (setq iterpoint (re-search-forward "^commit[ ]+\\([[:alnum:]]+\\)" nil t))
      (if (null iterpoint)
          (throw 'break nil))
      (setq commit-beginning (match-beginning 0))
      (setq commit-id (match-string 1))
      (logi "commit-id:%s" commit-id)

      (re-search-forward "^\\(commit[ ]+\\)\\([[:alnum:]]+\\)" nil t)
      (setq commit-end (match-beginning 0))
      (if (= commit-beginning commit-end)
          (setq commit-end (point-max)))
      (logi "commit form %d:%d" commit-beginning commit-end)
      (goto-char iterpoint)

      (setq iterpoint
            (re-search-forward "^Author:[ ]+\\([[:graph:]]+\\) [[:graph:]]+" nil t))
      (if (null iterpoint)
          (throw 'break nil))
      (setq commit-author (match-string 1))
      (logi "commit-author:%s" commit-author)

      (setq iterpoint (re-search-forward "^Date:[ ]+\\([[:print:]]+\\) \+.+" nil t))
      (if (null iterpoint)
          (throw 'break nil))
      (setq commit-date
            (substring
             (shell-command-to-string
              (concat "date \"+%Y-%m-%d_%H:%M:%S\" -d \"" (match-string 1) "\""))
             0 -1))
      (logi "commit-date:%s" commit-date)
      (setq commit-log-beginning iterpoint)

      (setq commit-log
            (replace-regexp-in-string
             "^    " ""
             (substring
              (buffer-substring commit-log-beginning commit-end) 1 -1)))
      (setq commit-type-match (string-match "^[[:alpha:]]+!*:" commit-log))
      (if commit-type-match
        (setq commit-type
              (substring commit-log commit-type-match (- (match-end 0) 1)))
        (setq commit-type-match (string-match "^[[:alpha:]]+([[:alpha:]]+)!*:"
                                              commit-log)))
      (if commit-type-match
        (setq commit-type
              (substring commit-log commit-type-match (- (match-end 0) 1)))
        (setq commit-type "nil"))
      (logd "commit-type:%s" commit-type)
      (cond
       ((null commit-type) ())
       ((string-prefix-p "milestone" commit-type)
        (setq commit-log
              (replace-regexp-in-string (concat "^" commit-type ":")
                                        (concat "* /*" commit-type "*/")
                                        commit-log)))
       ((string-prefix-p "release" commit-type)
        (setq commit-log
              (replace-regexp-in-string (concat "^" commit-type ":")
                                        (concat "* /*" commit-type "*/")
                                        commit-log)))
       ((string-prefix-p "developing" commit-type)
        (setq commit-log
              (replace-regexp-in-string (concat "^" commit-type ":")
                                        (concat "* /*" commit-type "*/")
                                        commit-log)))
       ((string-prefix-p "patch" commit-type)
        (setq commit-log
              (replace-regexp-in-string (concat "^" commit-type ":")
                                        (concat "* /*" commit-type "*/")
                                        commit-log)))
       ((stringp commit-type)
        (setq commit-log
              (replace-regexp-in-string (concat "^" commit-type ":")
                                        (concat "- /*" commit-type "*/")
                                        commit-log))))
      (logi "commit-log:\n%s" commit-log)

      ;; write to output buffer
      (set-buffer out-buffer)
      (set-buffer-multibyte t)
      (insert (concat commit-log "\n\n"))

      ;; switch to input buffer
      (set-buffer in-buffer))))

;; save
(set-buffer out-buffer)
(save-buffer)
(org-md-export-to-markdown)

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
