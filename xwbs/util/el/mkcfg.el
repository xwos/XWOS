#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to generate XWOS configurations and makefile
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
(defvar XWOS-opt-srcpath nil "Path of XWOS directory")
(defvar XWOS-opt-cfgdir nil "Path of configurations directory")
(defvar XWOS-opt-wkspc nil "Path of workspace directory")
(defvar XWOS-cfg "XWOS.cfg" "Name of XWOS.cfg")
(defvar XWOS-cfg-rs "XWOS.cfg.rs" "Name of XWOS.cfg.rs")
(logi "script path: %s" elpath)
(logd "argv: %s" argv)

(let ()
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      ;; (logd "option:%s" option)
      (cond
       ;; --XWOS foo
       ;; -s foo
       ;; --XWOS=foo
       ((string= option "--XWOS") (setq XWOS-opt-srcpath (pop argv)))
       ((string= option "-s") (setq XWOS-opt-srcpath (pop argv)))
       ((string-match "\\`--XWOS=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq XWOS-opt-srcpath (match-string 1 option)))

       ;; --cfgdir foo
       ;; -c foo
       ;; --cfgdir=foo
       ((string= option "--cfgdir") (setq XWOS-opt-cfgdir (pop argv)))
       ((string= option "-c") (setq XWOS-opt-cfgdir (pop argv)))
       ((string-match "\\`--cfgdir=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq XWOS-opt-cfgdir (match-string 1 option)))

       ;; --wkspc foo
       ;; -w foo
       ;; --wkspc=foo
       ((string= option "--wkspc") (setq XWOS-opt-wkspc (pop argv)))
       ((string= option "-w") (setq XWOS-opt-wkspc (pop argv)))
       ((string-match "\\`--wkspc=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq XWOS-opt-wkspc (match-string 1 option)))

       ;; Unknown option
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL)))))

  (unless (> (length XWOS-opt-srcpath) 0)
    (loge "Missing argument for --XWOS!")
    (kill-emacs EINVAL))
  (unless (> (length XWOS-opt-cfgdir) 0)
    (loge "Missing argument for --cfgdir!")
    (kill-emacs EINVAL))
  (unless (> (length XWOS-opt-wkspc) 0)
    (loge "Missing argument for --wkspc!")
    (kill-emacs EINVAL))

  (setq XWOS-opt-srcpath (directory-file-name XWOS-opt-srcpath))
  (setq XWOS-opt-cfgdir (directory-file-name XWOS-opt-cfgdir))
  (setq XWOS-opt-wkspc (directory-file-name XWOS-opt-wkspc)))

;;;;;;;; ;;;;;;;; ;;;;;;;; Get infomation ;;;;;;;; ;;;;;;;; ;;;;;;;;
;; Get top directory
(setq current-path (expand-file-name "."))
(logi "Current Path: %s" current-path)
(setq XWOS-srcpath
      (expand-file-name XWOS-opt-srcpath (file-name-as-directory current-path)))
(logi "XWOS SrcPath: %s" XWOS-srcpath)
(setq XWOS-rel-srcpath (file-relative-name XWOS-srcpath current-path))
(logi "XWOS Relative SrcPath: %s" XWOS-rel-srcpath)
(logi "XWOS CfgPath: %s" XWOS-opt-cfgdir)
(logi "XWOS Workspace: %s" XWOS-opt-wkspc)
(setq XWOS-obj-dir-rel-wkspcobj-dir
      (directory-file-name
       (let (result)
         (dolist (elt (split-string XWOS-rel-srcpath "/") result)
           (if (not (string= elt ".."))
               (setq result (concat result elt "/"))))
         (if (null result)
             (setq result "XWOS"))
         result)))
(logi "XWOS ObjPath rel workspace/obj: %s" XWOS-obj-dir-rel-wkspcobj-dir)

(setq XWOS-obj-dir-back-wkspcobj-dir
      (directory-file-name
       (let (result)
         (dolist (elt (split-string XWOS-obj-dir-rel-wkspcobj-dir "/") result)
           (setq result (concat result "../")))
         (if (null result)
             (setq result ".."))
         result)))
(logi "XWOS ObjPath back to workspace/obj: %s" XWOS-obj-dir-back-wkspcobj-dir)

(logi "XWOS.cfg: %s" XWOS-cfg)
(logi "XWOS.cfg.rs: %s" XWOS-cfg-rs)
(setq XWOS-cfg-buffer (find-file-noselect (concat XWOS-opt-wkspc "/" XWOS-cfg)))
(setq XWOS-cfg-rs-buffer (find-file-noselect (concat XWOS-opt-wkspc "/" XWOS-cfg-rs)))
(setq XWOS-autogen-header-buffer (find-file-noselect (concat XWOS-opt-cfgdir "/autogen.h")))
(setq XWOS-envrc-buffer (find-file-noselect (concat XWOS-opt-wkspc "/env.rc")))

(defvar project-cfg-h (concat XWOS-opt-cfgdir "/" "project.h") "Path of cfg/project.h")
(defvar arch-cfg-h (concat XWOS-opt-cfgdir "/" "arch.h") "Path of cfg/arch.h")
(defvar cpu-cfg-h (concat XWOS-opt-cfgdir "/" "cpu.h") "Path of cfg/cpu.h")
(defvar soc-cfg-h (concat XWOS-opt-cfgdir "/" "soc.h") "Path of cfg/soc.h")
(defvar brd-cfg-h (concat XWOS-opt-cfgdir "/" "board.h") "Path of cfg/board.h")
(defvar xwos-cfg-h (concat XWOS-opt-cfgdir "/" "xwos.h") "Path of cfg/xwos.h")
(defvar xwcd-cfg-h (concat XWOS-opt-cfgdir "/" "xwcd.h") "Path of cfg/xwcd.h")
(defvar xwmd-cfg-h (concat XWOS-opt-cfgdir "/" "xwmd.h") "Path of cfg/xwmd.h")
(defvar xwem-cfg-h (concat XWOS-opt-cfgdir "/" "xwem.h") "Path of cfg/xwem.h")
(defvar xwam-cfg-h (concat XWOS-opt-cfgdir "/" "xwam.h") "Path of cfg/xwam.h")
(defvar oem-cfg-h (concat XWOS-opt-cfgdir "/" "oem.h") "Path of cfg/oem.h")
(setq project-cfg-h-buffer (find-file-noselect project-cfg-h))
(setq arch-cfg-h-buffer (find-file-noselect arch-cfg-h))
(setq cpu-cfg-h-buffer (find-file-noselect cpu-cfg-h))
(setq soc-cfg-h-buffer (find-file-noselect soc-cfg-h))
(setq brd-cfg-h-buffer (find-file-noselect brd-cfg-h))
(setq xwos-cfg-h-buffer (find-file-noselect xwos-cfg-h))
(setq xwcd-cfg-h-buffer (find-file-noselect xwcd-cfg-h))
(setq xwmd-cfg-h-buffer (find-file-noselect xwmd-cfg-h))
(setq xwem-cfg-h-buffer (find-file-noselect xwem-cfg-h))
(setq xwam-cfg-h-buffer (find-file-noselect xwam-cfg-h))
(setq oem-cfg-h-buffer (find-file-noselect oem-cfg-h))

;; Get base Info
(set-buffer project-cfg-h-buffer)
(set-buffer-multibyte nil)

(logi "host: %s" (symbol-name system-type))

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_ARCH\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-arch (match-string 2))
(logi "ARCH: %s" XWOS-cfg-arch)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_SUBARCH\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-subarch (match-string 2))
(logi "Sub-ARCH: %s" XWOS-cfg-subarch)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_COMPILER\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-compiler (match-string 2))
(logi "compiler: %s" XWOS-cfg-compiler)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_LIBC\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-libc (match-string 2))
(logi "libc: %s" XWOS-cfg-libc)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XWCFG_LDSCRIPT[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XWOS-cfg-lds "")
    (setq XWOS-cfg-lds (match-string 1))))
(logi "XWOS-cfg-lds: %s" XWOS-cfg-lds)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_CPU\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-cpu (match-string 2))
(logi "CPU: %s" XWOS-cfg-cpu)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_SOC\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-soc (match-string 2))
(logi "SOC: %s" XWOS-cfg-soc)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_BOARD\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-brd (match-string 2))
(logi "board: %s" XWOS-cfg-brd)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_CORE\\)[ \t]+\\(.+\\)")
(setq XWOS-cfg-core (match-string 2))
(logi "core: %s" XWOS-cfg-core)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_XWMD\\)[\t ]+\\([10]\\)" nil t)
(setq XWOS-cfg-xwmd
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "middleware: %s" XWOS-cfg-xwmd)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_XWEM\\)[\t ]+\\([10]\\)" nil t)
(setq XWOS-cfg-xwem
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "external: %s" XWOS-cfg-xwem)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XWCFG_XWAM\\)[\t ]+\\([10]\\)" nil t)
(setq XWOS-cfg-xwam
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "application: %s" XWOS-cfg-xwam)

(goto-char (point-min))
(let (rc)
  (setq rc (re-search-forward
            "^#define[ \t]+XWCFG_OEMPATH[ \t]+\\(.+\\)" nil t))
  (if (null rc)
      (setq XWOS-cfg-oempath "")
      (setq XWOS-cfg-oempath (match-string 1))))
(logi "XWOS-cfg-oempath: %s" XWOS-cfg-oempath)

;; Get directories Info
(setq XWOS-xwos-dir (concat "xwos"))
(logi "XWOS-xwos-dir: %s" XWOS-xwos-dir)
(setq XWOS-xwcd-dir (concat "xwcd"))
(logi "XWOS-xwcd-dir: %s" XWOS-xwcd-dir)
(setq XWOS-xwmd-dir (concat "xwmd"))
(logi "XWOS-xwmd-dir: %s" XWOS-xwmd-dir)
(setq XWOS-xwem-dir (concat "xwem"))
(logi "XWOS-xwem-dir: %s" XWOS-xwem-dir)
(setq XWOS-xwam-dir (concat "xwam"))
(logi "XWOS-xwam-dir: %s" XWOS-xwam-dir)
(when (string= XWOS-cfg-compiler "clang")
  (setq XWOS-cfg-compiler "llvm"))
(setq XWOS-arch-dir (concat "xwcd/soc/" XWOS-cfg-arch "/" XWOS-cfg-subarch))
(logi "XWOS-arch-dir: %s" XWOS-arch-dir)
(setq XWOS-cpu-dir (concat XWOS-arch-dir "/" XWOS-cfg-cpu))
(logi "XWOS-cpu-dir: %s" XWOS-cpu-dir)
(setq XWOS-soc-dir (concat XWOS-cpu-dir "/" XWOS-cfg-soc))
(logi "XWOS-soc-dir: %s" XWOS-soc-dir)
(setq XWOS-brd-dir (file-relative-name current-path XWOS-srcpath))
(logi "XWOS-brd-dir: %s" XWOS-brd-dir)
(setq XWOS-bm-dir (concat XWOS-brd-dir "/bm"))
(logi "XWOS-bm-dir: %s" XWOS-bm-dir)
(setq XWOS-wkspc-dir
      (if (file-name-absolute-p XWOS-opt-wkspc)
          XWOS-opt-wkspc
          (concat XWOS-brd-dir "/" XWOS-opt-wkspc)))
(logi "XWOS-wkspc-dir: %s" XWOS-wkspc-dir)
(setq XWOS-wkspcobj-dir (concat XWOS-wkspc-dir "/obj"))
(logi "XWOS-wkspcobj-dir: %s" XWOS-wkspcobj-dir)
(setq XWOS-obj-dir (concat XWOS-wkspcobj-dir "/" XWOS-obj-dir-rel-wkspcobj-dir))
(logi "XWOS-obj-dir: %s" XWOS-obj-dir)
(setq XWOS-oemobj-dir (concat XWOS-wkspcobj-dir "/oem"))
(logi "XWOS-oemobj-dir: %s" XWOS-oemobj-dir)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate XWOS.cfg ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer XWOS-cfg-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert "## ******** ******** ******** ******** basic info ******** ******** ******** ******** ##\n")
(insert (concat "XWCFG_HOSTOS := " (symbol-name system-type) "\n"))
(insert (concat "XWCFG_ARCH := " XWOS-cfg-arch "\n"))
(insert (concat "XWCFG_SUBARCH := " XWOS-cfg-subarch "\n"))
(insert (concat "XWCFG_COMPILER := " XWOS-cfg-compiler "\n"))
(insert (concat "XWCFG_LIBC := " XWOS-cfg-libc "\n"))
(insert (concat "XWCFG_LDSCRIPT := " XWOS-cfg-lds "\n"))
(insert (concat "XWCFG_CPU := " XWOS-cfg-cpu "\n"))
(insert (concat "XWCFG_SOC := " XWOS-cfg-soc "\n"))
(insert (concat "XWCFG_BRD := " XWOS-cfg-brd "\n"))
(insert (concat "XWCFG_CORE := " XWOS-cfg-core "\n"))
(insert (concat "XWCFG_XWMD := " XWOS-cfg-xwmd "\n"))
(insert (concat "XWCFG_XWEM := " XWOS-cfg-xwem "\n"))
(insert (concat "XWCFG_XWAM := " XWOS-cfg-xwam "\n"))
(insert "## ******** ******** ******** ******** directory info ******** ******** ******** ******** ##\n")
(insert (concat "XWOS_PATH := " XWOS-srcpath "\n"))
(insert (concat "XWOS_RELPATH := " XWOS-rel-srcpath "\n"))
(insert (concat "XWOS_OBJ_DIR_REL_WKSPCOBJ_DIR := " XWOS-obj-dir-rel-wkspcobj-dir "\n"))
(insert (concat "XWOS_OBJ_DIR_BACK_WKSPCOBJ_DIR := " XWOS-obj-dir-back-wkspcobj-dir "\n"))
(insert (concat "XWOS_OS_DIR := " XWOS-xwos-dir "\n"))
(insert (concat "XWOS_XWCD_DIR := " XWOS-xwcd-dir "\n"))
(insert (concat "XWOS_XWMD_DIR := " XWOS-xwmd-dir "\n"))
(insert (concat "XWOS_XWEM_DIR := " XWOS-xwem-dir "\n"))
(insert (concat "XWOS_XWAM_DIR := " XWOS-xwam-dir "\n"))
(insert (concat "XWOS_ARCH_DIR := " XWOS-arch-dir "\n"))
(insert (concat "XWOS_CPU_DIR := " XWOS-cpu-dir "\n"))
(insert (concat "XWOS_SOC_DIR := " XWOS-soc-dir "\n"))
(insert (concat "XWOS_BRD_DIR := " XWOS-brd-dir "\n"))
(insert (concat "XWOS_BM_DIR := " XWOS-bm-dir "\n"))
(insert (concat "XWOS_OEM_DIR := " XWOS-brd-dir "/" XWOS-cfg-oempath "\n"))
(insert (concat "XWOS_WKSPC_DIR := " XWOS-wkspc-dir "\n"))
(insert (concat "XWOS_WKSPCOBJ_DIR := " XWOS-wkspcobj-dir "\n"))
(insert (concat "XWOS_OBJ_DIR := " XWOS-obj-dir "\n"))
(insert (concat "XWOS_OEMOBJ_DIR := " XWOS-oemobj-dir "\n"))
(insert "## ******** ******** ******** ******** ARCH ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer arch-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer arch-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** CPU ******** ******** ******** ******** ##\n")

(set-buffer cpu-cfg-h-buffer)
(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+CPUCFG_L1_CACHELINE_SIZE[ \t]+(*\\([0-9]+\\))*" nil t))
  (if (not (null rc))
      (progn
        (setq cpucfg-l1-cacheline-size (match-string 1))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat "CPUCFG_L1_CACHELINE_SIZE := " cpucfg-l1-cacheline-size "\n")))))

(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer cpu-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer cpu-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** SOC ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer soc-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer soc-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** Chip & Device ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer xwcd-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer xwcd-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** Board ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer brd-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer brd-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** XWOS Kernel ******** ******** ******** ******** ##\n")

(set-buffer xwos-cfg-h-buffer)
(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XWMMCFG_ALIGNMENT[ \t]+(*\\([0-9]+\\))*" nil t))
  (if (not (null rc))
      (progn
        (setq xwmmcfg-alignment (match-string 1))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat "XWMMCFG_ALIGNMENT := " xwmmcfg-alignment "\n")))))

(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer xwos-cfg-h-buffer)
  (while loopflg
    (goto-char iterpoint)
    (setq iterpoint (re-search-forward
                     "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
    (if (null iterpoint)
        (setq iterpoint (point-max)))
    (if (< iterpoint (point-max))
      (progn
        (setq item (match-string 1))
        (setq cfg (if (string= (match-string 2) "1") "y" "n"))
        (set-buffer XWOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer xwos-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** XWOS Middleware ******** ******** ******** ******** ##\n")
(if (string= XWOS-cfg-xwmd "y")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer xwmd-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XWOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwmd-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** External Module ******** ******** ******** ******** ##\n")
(if (string= XWOS-cfg-xwem "y")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer xwem-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XWOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwem-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** APP Module ******** ******** ******** ******** ##\n")
(if (string= XWOS-cfg-xwam "y")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer xwam-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XWOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwam-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XWOS-cfg-buffer)
(insert "## ******** ******** ******** ******** OEM Module ******** ******** ******** ******** ##\n")
(if (string> XWOS-cfg-oempath "")
    (let (item cfg (loopflg t) (iterpoint 1))
      (set-buffer oem-cfg-h-buffer)
      (while loopflg
        (goto-char iterpoint)
        (setq iterpoint (re-search-forward
                         "^#define[ \t]+\\([A-Za-z0-9_]+\\)[\t ]+\\([10]\\)" nil t))
        (if (null iterpoint)
            (setq iterpoint (point-max)))
        (if (< iterpoint (point-max))
            (progn
              (setq item (match-string 1))
              (setq cfg (if (string= (match-string 2) "1") "y" "n"))
              (set-buffer XWOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer oem-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XWOS-cfg-buffer)

;; save file
(save-buffer)


;;;;;;;; ;;;;;;;; ;;;;;;;; generate XWOS.cfg.rs ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer XWOS-cfg-rs-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert "// ******** ******** ******** ******** CPU ******** ******** ******** ********\n")

(set-buffer cpu-cfg-h-buffer)
(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+CPUCFG_L1_CACHELINE_SIZE[ \t]+(*\\([0-9]+\\))*" nil t))
  (if (not (null rc))
      (progn
        (setq cpucfg-l1-cacheline-size (match-string 1))
        (set-buffer XWOS-cfg-rs-buffer)
        (insert (concat "pub const CPUCFG_L1_CACHELINE_SIZE: usize = " cpucfg-l1-cacheline-size ";\n")))))

(insert "// ******** ******** ******** ******** XWOS Kernel ******** ******** ******** ********\n")

(set-buffer xwos-cfg-h-buffer)
(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XWMMCFG_ALIGNMENT[ \t]+(*\\([0-9]+\\))*" nil t))
  (if (not (null rc))
      (progn
        (setq xwmmcfg-alignment (match-string 1))
        (set-buffer XWOS-cfg-rs-buffer)
        (insert (concat "pub const XWMMCFG_ALIGNMENT: usize = " xwmmcfg-alignment ";\n")))))

;; save file
(set-buffer XWOS-cfg-rs-buffer)
(save-buffer)


;;;;;;;; ;;;;;;;; ;;;;;;;; generate header ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer XWOS-autogen-header-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert (concat
"/* Don't edit this file, because it is generated by xwbs/mkcfg.el */

#ifndef __cfg_autogen_h__
#define __cfg_autogen_h__

#define XWCFG_ARCH__" XWOS-cfg-arch " 1
#define XWCFG_SUBARCH__" XWOS-cfg-subarch " 1
#define XWCFG_COMPILER__" XWOS-cfg-compiler " 1
#define XWCFG_LIBC__" XWOS-cfg-libc " 1
#define XWCFG_CPU__" XWOS-cfg-cpu " 1
#define XWCFG_SOC__" XWOS-cfg-soc " 1
#define XWCFG_BOARD__" XWOS-cfg-brd " 1
#define XWCFG_CORE__" XWOS-cfg-core " 1

#endif /* cfg/autogen.h */
"))
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate env.rc ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer XWOS-envrc-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert
"# @file
# @brief environment script
# @note
# This file is generated by mkcfg.el
#
")

(insert "## ******** ******** ******** ******** basic info ******** ******** ******** ******** ##\n")
(insert (concat "export XWCFG_HOSTOS=" (symbol-name system-type) "\n"))
(insert (concat "export XWCFG_ARCH=" XWOS-cfg-arch "\n"))
(insert (concat "export XWCFG_SUBARCH=" XWOS-cfg-subarch "\n"))
(insert (concat "export XWCFG_COMPILER=" XWOS-cfg-compiler "\n"))
(insert (concat "export XWCFG_LIBC=" XWOS-cfg-libc "\n"))
(insert (concat "export XWCFG_LDSCRIPT=" XWOS-cfg-lds "\n"))
(insert (concat "export XWCFG_CPU=" XWOS-cfg-cpu "\n"))
(insert (concat "export XWCFG_SOC=" XWOS-cfg-soc "\n"))
(insert (concat "export XWCFG_BRD=" XWOS-cfg-brd "\n"))
(insert (concat "export XWCFG_CORE=" XWOS-cfg-core "\n"))
(insert (concat "export XWCFG_XWMD=" XWOS-cfg-xwmd "\n"))
(insert (concat "export XWCFG_XWEM=" XWOS-cfg-xwem "\n"))
(insert (concat "export XWCFG_XWAM=" XWOS-cfg-xwam "\n"))
(insert "## ******** ******** ******** ******** directory info ******** ******** ******** ******** ##\n")
(insert (concat "export XWOS_PATH=" XWOS-srcpath "\n"))
(insert (concat "export XWOS_RELPATH=" XWOS-rel-srcpath "\n"))
(insert (concat "export XWOS_OBJ_DIR_REL_WKSPCOBJ_DIR=" XWOS-obj-dir-rel-wkspcobj-dir "\n"))
(insert (concat "export XWOS_OBJ_DIR_BACK_WKSPCOBJ_DIR=" XWOS-obj-dir-back-wkspcobj-dir "\n"))
(insert (concat "export XWOS_OS_DIR=" XWOS-xwos-dir "\n"))
(insert (concat "export XWOS_XWCD_DIR=" XWOS-xwcd-dir "\n"))
(insert (concat "export XWOS_XWMD_DIR=" XWOS-xwmd-dir "\n"))
(insert (concat "export XWOS_XWEM_DIR=" XWOS-xwem-dir "\n"))
(insert (concat "export XWOS_XWAM_DIR=" XWOS-xwam-dir "\n"))
(insert (concat "export XWOS_ARCH_DIR=" XWOS-arch-dir "\n"))
(insert (concat "export XWOS_CPU_DIR=" XWOS-cpu-dir "\n"))
(insert (concat "export XWOS_SOC_DIR=" XWOS-soc-dir "\n"))
(insert (concat "export XWOS_BRD_DIR=" XWOS-brd-dir "\n"))
(insert (concat "export XWOS_BM_DIR=" XWOS-bm-dir "\n"))
(insert (concat "export XWOS_OEM_DIR=" XWOS-brd-dir "/" XWOS-cfg-oempath "\n"))
(insert (concat "export XWOS_WKSPC_DIR=" XWOS-wkspc-dir "\n"))
(insert (concat "export XWOS_WKSPCOBJ_DIR=" XWOS-wkspcobj-dir "\n"))
(insert (concat "export XWOS_OBJ_DIR=" XWOS-obj-dir "\n"))
(insert (concat "export XWOS_OEMOBJ_DIR=" XWOS-oemobj-dir "\n"))
(insert "## ******** ******** ******** ******** includes ******** ******** ******** ******** ##\n")
(insert "source ${XWOS_PATH}/xwbs/util/sh/xwenv.sh\n")

(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
