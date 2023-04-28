#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to update XWOS version
;; @author
;; + 隐星魂 (Roy Sun) <xwos@xwos.tech>
;; @copyright
;; + Copyright © 2015 xwos.tech, All Rights Reserved.
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
(defconst cfg-log-lv 0)

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

;;;;;;;; ;;;;;;;; ;;;;;;;; misc function ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defun expand-directory (dir &rest arglist)
  (file-name-directory
   (expand-file-name
    (concat dir
            (let (result)
              (dolist (elt arglist result)
                (setq result (concat result elt))))
            "fakefile"))))

;;;;;;;; ;;;;;;;; ;;;;;;;; arguments ;;;;;;;; ;;;;;;;; ;;;;;;;;
(setq debug-on-error t)
(setq make-backup-files nil)
(defvar argc (length argv) "Count of arguments")
(defvar opt-major nil "increase major")
(defvar opt-minor nil "increase minor")
(defvar opt-revision nil "increase revision")
(defvar opt-phase "alpha" "version phase")
(defvar opt-phase-p nil "Is version phase setting?")
(defvar opt-reset nil "Reset change")
;;(logi "argv:%s" argv)

(catch 'break
  (pop argv)  ; Remove first "--" separator
  (while argv
    (let ((option (pop argv)))
      ;; (logd "option:%s" option)
      (cond
       ;; options after second "--"
       ((string= option "--") (throw 'break nil))
       ;; -M
       ;; --major
       ((string= option "--major") (setq opt-major t))
       ((string= option "-M") (setq opt-major t))
       ;; -m
       ;; --minor
       ((string= option "--minor") (setq opt-minor t))
       ((string= option "-m") (setq opt-minor t))
       ;; -v
       ;; --revision
       ((string= option "--revision") (setq opt-revision t))
       ((string= option "-v") (setq opt-revision t))
       ;; -p foo
       ;; --phase foo
       ;; --phase=foo
       ((string= option "--phase")
        (setq opt-phase (pop argv))
        (setq opt-phase-p t))
       ((string= option "-p")
        (setq opt-phase (pop argv))
        (setq opt-phase-p t))
       ((string-match "\\`--phase=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq opt-phase (match-string 1 option))
        (setq opt-phase-p t))
       ;; -r
       ;; --reset
       ((string= option "--reset") (setq opt-reset t))
       ((string= option "-r") (setq opt-reset t))
       ;; Unknown option --*
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))
       ;; Unknown option -*
       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))
       ;; Unknown option *
       (t
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))))))

;;;;;;;; ;;;;;;;; ;;;;;;;; env ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defvar XWOS-topdir (expand-directory elpath "../../../"))
(defvar XWOS-kndir (expand-directory XWOS-topdir "xwos/"))
(defvar XWOS-version-file (expand-file-name "lib/version.h" XWOS-kndir))

;;(logd "XWOS-topdir:%s" XWOS-topdir)
;;(logd "XWOS-version-file:%s" XWOS-version-file)

(setq version-file-buffer (find-file-noselect XWOS-version-file))
(set-buffer version-file-buffer)
(set-buffer-multibyte t)

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_MAJOR\\)[ \t]+\\(.+\\)")
(setq XWOS-version-major (match-string 2))

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_MINOR\\)[ \t]+\\(.+\\)")
(setq XWOS-version-minor (match-string 2))

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_REVISION\\)[ \t]+\\(.+\\)")
(setq XWOS-version-revision (match-string 2))

(goto-char (point-min))
(re-search-forward "^#define[ \t]+\\(XWOS_VERSION_PHASE\\)[ \t]+\\(.+\\)")
(setq XWOS-version-phase (match-string 2))

(logi "current version: V%s.%s.%s.%s"
      XWOS-version-major XWOS-version-minor XWOS-version-revision XWOS-version-phase)

(if opt-reset
  ;; then
  (let (recent-git-commit vnum-major vnum-minor vnum-revision vstr bvstr)
    (setq recent-git-commit
          (with-temp-buffer
            (call-process "git" nil t nil "log" "-1")
            (buffer-string)))
    (setq vnum-major (string-to-number XWOS-version-major))
    (setq vnum-minor (string-to-number XWOS-version-minor))
    (setq vnum-revision (string-to-number XWOS-version-revision))
    (setq vstr (concat XWOS-version-major "."
                       XWOS-version-minor "."
                       XWOS-version-revision))
    (setq bvstr (concat XWOS-version-major "." XWOS-version-minor))
    (cond
     ((or (string-match "release: :bookmark:" recent-git-commit)
          (string-match "milestone: :bookmark:" recent-git-commit))
      (call-process "git" nil nil nil "branch" "-D" (concat "XWOS-V" bvstr))
      (call-process "git" nil nil nil "tag" "-d" (concat "XWOS-V" vstr))
      (call-process "git" nil nil nil "reset" "HEAD^")
      (call-process "git" nil nil nil "checkout" XWOS-version-file)
      (logi "Reset version .... [OK]"))
     ((string-match "revision: :bookmark:" recent-git-commit)
      (call-process "git" nil nil nil "tag" "-d" (concat "XWOS-V" vstr))
      (call-process "git" nil nil nil "reset" "HEAD^")
      (call-process "git" nil nil nil "checkout" XWOS-version-file)
      (logi "Reset version .... [OK]"))
     ((or (string-match "alpha: :bookmark:" recent-git-commit)
          (string-match "beta: :bookmark:" recent-git-commit)
          (string-match "rc: :bookmark:" recent-git-commit))
      (call-process "git" nil nil nil "reset" "HEAD^")
      (call-process "git" nil nil nil "checkout" XWOS-version-file)
      (logi "Reset version .... [OK]"))
     (t (loge "[FAILED] The recent git commit is not a version update!"))))
  ;; else
  (let (vnum-major vnum-minor vnum-revision vstr bvstr)
    (setq vnum-major (string-to-number XWOS-version-major))
    (setq vnum-minor (string-to-number XWOS-version-minor))
    (setq vnum-revision (string-to-number XWOS-version-revision))
    (cond
     ((or (string= opt-phase "milestone") ;; case opt-phase == "milestone"
          (string= opt-phase "release")) ;; case opt-phase == "release"
      (setq XWOS-version-phase opt-phase)
      (setq vstr (concat XWOS-version-major "."
                         XWOS-version-minor "."
                         XWOS-version-revision))
      (setq bvstr (concat XWOS-version-major "." XWOS-version-minor))
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_PHASE[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-phase)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XWOS-version-file)
      (call-process "git" nil nil nil "commit" "-m" (concat opt-phase ": :bookmark: XWOS-V" vstr))
      (call-process "git" nil nil nil "tag" "-a" "-m" (concat "XWOS-V" vstr) (concat "XWOS-V" vstr))
      (call-process "git" nil nil nil "branch" (concat "XWOS-V" bvstr))
      (logi "update phase: %s" XWOS-version-phase))
     (opt-major ;; case opt-major
      (setq XWOS-version-major (number-to-string (+ vnum-major 1)))
      (setq XWOS-version-minor "0")
      (setq XWOS-version-revision "0")
      (setq XWOS-version-phase opt-phase)
      (setq vstr (concat XWOS-version-major "."
                         XWOS-version-minor "."
                         XWOS-version-revision))
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_MAJOR[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-major)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_MINOR[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-minor)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_REVISION[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-revision)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_PHASE[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-phase)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XWOS-version-file)
      (call-process "git" nil nil nil "commit" "-m" (concat opt-phase ": :bookmark: XWOS-V" vstr))
      (logi "update version: V%s.%s" vstr opt-phase))
     (opt-minor ;; case opt-minor
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (setq vnum-minor (+ vnum-minor 1))
      (setq XWOS-version-minor (number-to-string vnum-minor))
      (setq XWOS-version-revision "0")
      (setq XWOS-version-phase opt-phase)
      (setq vstr (concat XWOS-version-major "."
                         XWOS-version-minor "."
                         XWOS-version-revision))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_MINOR[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-minor)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_REVISION[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-revision)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_PHASE[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-phase)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XWOS-version-file)
      (call-process "git" nil nil nil "commit" "-m" (concat opt-phase ": :bookmark: XWOS-V" vstr))
      (logi "update version: V%s.%s" vstr opt-phase))
     (opt-revision ;; case opt-revision
      (setq XWOS-version-revision (number-to-string (+ vnum-revision 1)))
      (setq XWOS-version-phase "revision")
      (setq vstr (concat XWOS-version-major "."
                         XWOS-version-minor "."
                         XWOS-version-revision))
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_REVISION[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-revision)))
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_PHASE[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-phase)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XWOS-version-file)
      (call-process "git" nil nil nil "commit" "-m" (concat "revision: :bookmark: XWOS-V" vstr))
      (call-process "git" nil nil nil "tag" "-a" "-m" (concat "XWOS-V" vstr) (concat "XWOS-V" vstr))
      (logi "update version: V%s" vstr))
     (opt-phase-p ;; case opt-phase-p
      (setq XWOS-version-phase opt-phase)
      (setq vstr (concat XWOS-version-major "."
                         XWOS-version-minor "."
                         XWOS-version-revision))
      (set-buffer version-file-buffer)
      (set-buffer-multibyte t)
      (goto-char (point-min))
      (while (re-search-forward
              "^\\(#define[ \t]+XWOS_VERSION_PHASE[ \t]+\\)\\(.+\\)"
              nil t)
        (replace-match (concat "\\1" XWOS-version-phase)))
      (save-buffer)
      (call-process "git" nil nil nil "add" XWOS-version-file)
      (call-process "git" nil nil nil "commit" "-m" (concat opt-phase ": :bookmark: XWOS-V" vstr))
      (logi "update phase: %s" XWOS-version-phase)))))

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
