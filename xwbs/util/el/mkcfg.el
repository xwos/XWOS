#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to generate XuanWuOS configurations and makefile
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
(defconst cfg-log-lv 1)

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
(defvar XuanWuOS-opt-srcpath nil "Path of XuanWuOS directory")
(defvar XuanWuOS-opt-cfgdir nil "Path of configurations directory")
(defvar XuanWuOS-opt-wkspc nil "Path of workspace directory")
(defvar XuanWuOS-cfg ".cfg" "Name of XuanWuOS.cfg")
(logi "script path:%s" elpath)
(logd "argv:%s" argv)

(let ((options-done nil))
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      ;; (logd "option:%s" option)
      (cond
       (options-done (setq XuanWuOS-cfg option))

       ;; options after "--"
       ((string= option "--") (setq options-done t))

       ;; --XuanWuOS foo
       ;; -s foo
       ;; --XuanWuOS=foo
       ((string= option "--XuanWuOS") (setq XuanWuOS-opt-srcpath (pop argv)))
       ((string= option "-s") (setq XuanWuOS-opt-srcpath (pop argv)))
       ((string-match "\\`--XuanWuOS=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq XuanWuOS-opt-srcpath (match-string 1 option)))

       ;; --cfgdir foo
       ;; -c foo
       ;; --cfgdir=foo
       ((string= option "--cfgdir") (setq XuanWuOS-opt-cfgdir (pop argv)))
       ((string= option "-c") (setq XuanWuOS-opt-cfgdir (pop argv)))
       ((string-match "\\`--cfgdir=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq XuanWuOS-opt-cfgdir (match-string 1 option)))

       ;; --wkspc foo
       ;; -w foo
       ;; --wkspc=foo
       ((string= option "--wkspc") (setq XuanWuOS-opt-wkspc (pop argv)))
       ((string= option "-w") (setq XuanWuOS-opt-wkspc (pop argv)))
       ((string-match "\\`--wkspc=\\(\\(?:.\\|\n\\)*\\)\\'" option)
        (setq XuanWuOS-opt-wkspc (match-string 1 option)))

       ;; Unknown option
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ;; default
       (t (setq XuanWuOS-cfg option)))))

  (unless (> (length XuanWuOS-opt-srcpath) 0)
    (loge "Missing argument for --XuanWuOS!")
    (kill-emacs EINVAL))
  (unless (> (length XuanWuOS-opt-cfgdir) 0)
    (loge "Missing argument for --cfgdir!")
    (kill-emacs EINVAL))
  (unless (> (length XuanWuOS-opt-wkspc) 0)
    (loge "Missing argument for --wkspc!")
    (kill-emacs EINVAL))

  (setq XuanWuOS-opt-srcpath (directory-file-name XuanWuOS-opt-srcpath))
  (setq XuanWuOS-opt-cfgdir (directory-file-name XuanWuOS-opt-cfgdir))
  (setq XuanWuOS-opt-wkspc (directory-file-name XuanWuOS-opt-wkspc)))

;;;;;;;; ;;;;;;;; ;;;;;;;; Get infomation ;;;;;;;; ;;;;;;;; ;;;;;;;;
;; Get top directory
(setq current-path (expand-file-name "."))
(logi "Current Path:%s" current-path)
(setq XuanWuOS-srcpath
      (expand-file-name XuanWuOS-opt-srcpath (file-name-as-directory current-path)))
(logi "XuanWuOS SrcPath:%s" XuanWuOS-srcpath)
(setq XuanWuOS-rel-srcpath (file-relative-name XuanWuOS-srcpath current-path))
(logi "XuanWuOS Relative SrcPath:%s" XuanWuOS-rel-srcpath)
(logi "XuanWuOS CfgPath:%s" XuanWuOS-opt-cfgdir)
(logi "XuanWuOS Workspace:%s" XuanWuOS-opt-wkspc)
(setq XuanWuOS-forward-srcpath
      (directory-file-name
       (let (result)
         (dolist (elt (split-string XuanWuOS-rel-srcpath "/") result)
           (if (not (string= elt ".."))
               (setq result (concat result elt "/"))))
         (if (null result)
             (setq result "XuanWuOS"))
         result)))
(logi "XuanWuOS Forward SrcPath:%s" XuanWuOS-forward-srcpath)

(setq XuanWuOS-backward-srcpath
      (directory-file-name
       (let (result)
         (dolist (elt (split-string XuanWuOS-forward-srcpath "/") result)
           (setq result (concat result "../")))
         (if (null result)
             (setq result ".."))
         result)))
(logi "XuanWuOS Backward SrcPath:%s" XuanWuOS-backward-srcpath)

(logi "XuanWuOS.cfg:%s" XuanWuOS-cfg)
(setq XuanWuOS-cfg-buffer (find-file-noselect (concat XuanWuOS-opt-wkspc "/" XuanWuOS-cfg)))
(setq xwos-build-mk-buffer (find-file-noselect (concat XuanWuOS-opt-wkspc "/makefile")))
(setq xwos-autogen-header-buffer (find-file-noselect (concat XuanWuOS-opt-wkspc "/autogen.h")))
(setq xwos-envrc-buffer (find-file-noselect (concat XuanWuOS-opt-wkspc "/env.rc")))

(defvar XuanWuOS-cfg-h (concat XuanWuOS-opt-cfgdir "/" "XuanWuOS.h") "Path of cfg/XuanWuOS.h")
(defvar arch-cfg-h (concat XuanWuOS-opt-cfgdir "/" "arch.h") "Path of cfg/arch.h")
(defvar cpu-cfg-h (concat XuanWuOS-opt-cfgdir "/" "cpu.h") "Path of cfg/cpu.h")
(defvar soc-cfg-h (concat XuanWuOS-opt-cfgdir "/" "soc.h") "Path of cfg/soc.h")
(defvar brd-cfg-h (concat XuanWuOS-opt-cfgdir "/" "board.h") "Path of cfg/board.h")
(defvar xwkn-cfg-h (concat XuanWuOS-opt-cfgdir "/" "xwos.h") "Path of cfg/xwos.h")
(defvar xwcd-cfg-h (concat XuanWuOS-opt-cfgdir "/" "xwcd.h") "Path of cfg/xwcd.h")
(defvar xwmd-cfg-h (concat XuanWuOS-opt-cfgdir "/" "xwmd.h") "Path of cfg/xwmd.h")
(defvar xwem-cfg-h (concat XuanWuOS-opt-cfgdir "/" "xwem.h") "Path of cfg/xwem.h")
(defvar xwam-cfg-h (concat XuanWuOS-opt-cfgdir "/" "xwam.h") "Path of cfg/xwam.h")
(defvar oem-cfg-h (concat XuanWuOS-opt-cfgdir "/" "oem.h") "Path of cfg/oem.h")
(setq XuanWuOS-cfg-h-buffer (find-file-noselect XuanWuOS-cfg-h))
(setq arch-cfg-h-buffer (find-file-noselect arch-cfg-h))
(setq cpu-cfg-h-buffer (find-file-noselect cpu-cfg-h))
(setq soc-cfg-h-buffer (find-file-noselect soc-cfg-h))
(setq brd-cfg-h-buffer (find-file-noselect brd-cfg-h))
(setq xwkn-cfg-h-buffer (find-file-noselect xwkn-cfg-h))
(setq xwcd-cfg-h-buffer (find-file-noselect xwcd-cfg-h))
(setq xwmd-cfg-h-buffer (find-file-noselect xwmd-cfg-h))
(setq xwem-cfg-h-buffer (find-file-noselect xwem-cfg-h))
(setq xwam-cfg-h-buffer (find-file-noselect xwam-cfg-h))
(setq oem-cfg-h-buffer (find-file-noselect oem-cfg-h))

;; Get base Info
(set-buffer XuanWuOS-cfg-h-buffer)
(set-buffer-multibyte nil)

(logi "host:%s" (symbol-name system-type))

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_ARCH\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-arch (match-string 2))
(logi "ARCH:%s" XuanWuOS-cfg-arch)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_SUBARCH\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-subarch (match-string 2))
(logi "Sub-ARCH:%s" XuanWuOS-cfg-subarch)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_COMPILER\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-compiler (match-string 2))
(logi "compiler:%s" XuanWuOS-cfg-compiler)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_ELF_MK[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-elf-mk "")
    (setq XuanWuOS-cfg-elf-mk (match-string 1))))
(logi "XuanWuOS-cfg-elf-makefile:%s" XuanWuOS-cfg-elf-mk)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_XWMO_MK[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-xwmo-mk "")
    (setq XuanWuOS-cfg-xwmo-mk (match-string 1))))
(logi "XuanWuOS-cfg-xwmo-makefile:%s" XuanWuOS-cfg-xwmo-mk)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_XWMO_PREBUILT_MK[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-xwmo-prebuilt-mk "")
    (setq XuanWuOS-cfg-xwmo-prebuilt-mk (match-string 1))))
(logi "XuanWuOS-cfg-xwmo-makefile:%s" XuanWuOS-cfg-xwmo-prebuilt-mk)

(goto-char (point-min))
(let ((rc))
  (setq rc (re-search-forward "^#define[ \t]+XuanWuOS_CFG_LDSCRIPT[ \t]+\\(.+\\)" nil t))
  (if (null rc)
    (setq XuanWuOS-cfg-lds "")
    (setq XuanWuOS-cfg-lds (match-string 1))))
(logi "XuanWuOS-cfg-lds:%s" XuanWuOS-cfg-lds)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_CPU\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-cpu (match-string 2))
(logi "CPU:%s" XuanWuOS-cfg-cpu)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_SOC\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-soc (match-string 2))
(logi "SOC:%s" XuanWuOS-cfg-soc)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_BOARD\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-brd (match-string 2))
(logi "board:%s" XuanWuOS-cfg-brd)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_CORE\\)[ \t]+\\(.+\\)")
(setq XuanWuOS-cfg-core (match-string 2))
(logi "core:%s" XuanWuOS-cfg-core)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_XWMD\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-xwmd
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "middleware:%s" XuanWuOS-cfg-xwmd)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_XWEM\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-xwem
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "external:%s" XuanWuOS-cfg-xwem)

(goto-char (point-min))
(re-search-forward
 "^#define[ \t]+\\(XuanWuOS_CFG_XWAM\\)[\t ]+\\([10]\\)" nil t)
(setq XuanWuOS-cfg-xwam
      (cond
       ((string= (match-string 2) "1") "y")
       (t "n")))
(logi "application:%s" XuanWuOS-cfg-xwam)

(goto-char (point-min))
(let (rc)
  (setq rc (re-search-forward
            "^#define[ \t]+XuanWuOS_CFG_OEMPATH[ \t]+\\(.+\\)" nil t))
  (if (null rc)
      (setq XuanWuOS-cfg-oempath "")
      (setq XuanWuOS-cfg-oempath (match-string 1))))
(logi "XuanWuOS-cfg-oempath:%s" XuanWuOS-cfg-oempath)

;; Get directories Info
(setq xwos-kr-dir (concat "xwos"))
(logi "xwos-kr-dir:%s" xwos-kr-dir)
(setq xwos-cd-dir (concat "xwcd"))
(logi "xwos-cd-dir:%s" xwos-cd-dir)
(setq xwos-md-dir (concat "xwmd"))
(logi "xwos-md-dir:%s" xwos-md-dir)
(setq xwos-em-dir (concat "xwem"))
(logi "xwos-em-dir:%s" xwos-em-dir)
(setq xwos-am-dir (concat "xwam"))
(logi "xwos-am-dir:%s" xwos-am-dir)
(when (string= XuanWuOS-cfg-compiler "clang")
  (setq XuanWuOS-cfg-compiler "llvm"))
(setq xwos-arch-dir (concat "xwcd/soc/" XuanWuOS-cfg-arch "/" XuanWuOS-cfg-subarch))
(logi "xwos-arch-dir:%s" xwos-arch-dir)
(setq xwos-cpu-dir (concat xwos-arch-dir "/" XuanWuOS-cfg-cpu))
(logi "xwos-cpu-dir:%s" xwos-cpu-dir)
(setq xwos-soc-dir (concat xwos-cpu-dir "/" XuanWuOS-cfg-soc))
(logi "xwos-soc-dir:%s" xwos-soc-dir)
(setq xwos-brd-dir (file-relative-name current-path XuanWuOS-srcpath))
(logi "xwos-brd-dir:%s" xwos-brd-dir)
(setq xwos-bdl-dir (concat xwos-brd-dir "/bdl"))
(logi "xwos-bdl-dir:%s" xwos-bdl-dir)
(setq xwos-bm-dir (concat xwos-brd-dir "/bm"))
(logi "xwos-bm-dir:%s" xwos-bm-dir)
(setq xwos-wkspc-dir
      (if (file-name-absolute-p XuanWuOS-opt-wkspc)
          XuanWuOS-opt-wkspc
          (concat xwos-brd-dir "/" XuanWuOS-opt-wkspc)))
(logi "xwos-wkspc-dir:%s" xwos-wkspc-dir)
(setq xwos-obj-dir (concat xwos-wkspc-dir "/obj/" XuanWuOS-forward-srcpath))
(logi "xwos-obj-dir:%s" xwos-obj-dir)
(setq xwos-oemobj-dir (concat xwos-wkspc-dir "/obj/oem"))
(logi "xwos-oemobj-dir:%s" xwos-oemobj-dir)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate XuanWuOS.cfg ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer XuanWuOS-cfg-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert "## ******** ******** ******** ******** basic info ******** ******** ******** ******** ##\n")
(insert (concat "XuanWuOS_CFG_HOSTOS := " (symbol-name system-type) "\n"))
(insert (concat "XuanWuOS_CFG_ARCH := " XuanWuOS-cfg-arch "\n"))
(insert (concat "XuanWuOS_CFG_SUBARCH := " XuanWuOS-cfg-subarch "\n"))
(insert (concat "XuanWuOS_CFG_COMPILER := " XuanWuOS-cfg-compiler "\n"))
(insert (concat "XuanWuOS_CFG_LDSCRIPT := " XuanWuOS-cfg-lds "\n"))
(insert (concat "XuanWuOS_CFG_CPU := " XuanWuOS-cfg-cpu "\n"))
(insert (concat "XuanWuOS_CFG_SOC := " XuanWuOS-cfg-soc "\n"))
(insert (concat "XuanWuOS_CFG_BRD := " XuanWuOS-cfg-brd "\n"))
(insert (concat "XuanWuOS_CFG_CORE := " XuanWuOS-cfg-core "\n"))
(insert (concat "XuanWuOS_CFG_XWMD := " XuanWuOS-cfg-xwmd "\n"))
(insert (concat "XuanWuOS_CFG_XWEM := " XuanWuOS-cfg-xwem "\n"))
(insert (concat "XuanWuOS_CFG_XWAM := " XuanWuOS-cfg-xwam "\n"))
(insert (concat "XuanWuOS_CFG_ELF_MK := " XuanWuOS-cfg-elf-mk "\n"))
(insert (concat "XuanWuOS_CFG_XWMO_MK := " XuanWuOS-cfg-xwmo-mk "\n"))
(insert (concat "XuanWuOS_CFG_XWMO_PREBUILT_MK := " XuanWuOS-cfg-xwmo-prebuilt-mk "\n"))
(insert "## ******** ******** ******** ******** build Script info ******** ******** ******** ******** ##\n")
(insert (concat "XWBS_UTIL_MK_XWMO := " "xwbs/util/mk/xwmo.mk\n"))
(insert "## ******** ******** ******** ******** directory info ******** ******** ******** ******** ##\n")
(insert (concat "XuanWuOS_PATH := " XuanWuOS-srcpath "\n"))
(insert (concat "XuanWuOS_FWDPATH := " XuanWuOS-forward-srcpath "\n"))
(insert (concat "XuanWuOS_BWDPATH := " XuanWuOS-backward-srcpath "\n"))
(insert (concat "XuanWuOS_XWOS_DIR := " xwos-kr-dir "\n"))
(insert (concat "XuanWuOS_XWCD_DIR := " xwos-cd-dir "\n"))
(insert (concat "XuanWuOS_XWMD_DIR := " xwos-md-dir "\n"))
(insert (concat "XuanWuOS_XWEM_DIR := " xwos-em-dir "\n"))
(insert (concat "XuanWuOS_XWAM_DIR := " xwos-am-dir "\n"))
(insert (concat "XuanWuOS_ARCH_DIR := " xwos-arch-dir "\n"))
(insert (concat "XuanWuOS_CPU_DIR := " xwos-cpu-dir "\n"))
(insert (concat "XuanWuOS_SOC_DIR := " xwos-soc-dir "\n"))
(insert (concat "XuanWuOS_BRD_DIR := " xwos-brd-dir "\n"))
(insert (concat "XuanWuOS_BDL_DIR := " xwos-bdl-dir "\n"))
(insert (concat "XuanWuOS_BM_DIR := " xwos-bm-dir "\n"))
(insert (concat "XuanWuOS_OEM_DIR := " xwos-brd-dir "/" XuanWuOS-cfg-oempath "\n"))
(insert (concat "XuanWuOS_WKSPC_DIR := " xwos-wkspc-dir "\n"))
(insert (concat "XuanWuOS_OBJ_DIR := " xwos-obj-dir "\n"))
(insert (concat "XuanWuOS_OEMOBJ_DIR := " xwos-oemobj-dir "\n"))
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
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer arch-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** CPU ******** ******** ******** ******** ##\n")
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
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer cpu-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
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
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer soc-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
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
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer xwcd-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
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
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer brd-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** XWOS Kernel ******** ******** ******** ******** ##\n")
(let (item cfg (loopflg t) (iterpoint 1))
  (set-buffer xwkn-cfg-h-buffer)
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
        (set-buffer XuanWuOS-cfg-buffer)
        (insert (concat item " := " cfg "\n"))
        (set-buffer xwkn-cfg-h-buffer))
      (setq loopflg nil))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** XWOS Middleware ******** ******** ******** ******** ##\n")
(if (string= XuanWuOS-cfg-xwmd "y")
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
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwmd-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** External Module ******** ******** ******** ******** ##\n")
(if (string= XuanWuOS-cfg-xwem "y")
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
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwem-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** APP Module ******** ******** ******** ******** ##\n")
(if (string= XuanWuOS-cfg-xwam "y")
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
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer xwam-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)
(insert "## ******** ******** ******** ******** OEM Module ******** ******** ******** ******** ##\n")
(if (string> XuanWuOS-cfg-oempath "")
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
              (set-buffer XuanWuOS-cfg-buffer)
              (insert (concat item " := " cfg "\n"))
              (set-buffer oem-cfg-h-buffer))
          (setq loopflg nil)))))
(set-buffer XuanWuOS-cfg-buffer)

;; save file
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate header ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer xwos-autogen-header-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert (concat
"/* Don't edit this file, because it is generated by xwbs/mkcfg.el */

#ifndef __wkspc_autogen_h__
#define __wkspc_autogen_h__

#define XuanWuOS_CFG_ARCH__" XuanWuOS-cfg-arch " 1
#define XuanWuOS_CFG_SUBARCH__" XuanWuOS-cfg-subarch " 1
#define XuanWuOS_CFG_COMPILER__" XuanWuOS-cfg-compiler " 1
#define XuanWuOS_CFG_CPU__" XuanWuOS-cfg-cpu " 1
#define XuanWuOS_CFG_SOC__" XuanWuOS-cfg-soc " 1
#define XuanWuOS_CFG_BOARD__" XuanWuOS-cfg-brd " 1
#define XuanWuOS_CFG_CORE__" XuanWuOS-cfg-core " 1

#endif /* wkspc/autogen.h */
"))
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate build makefile ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer xwos-build-mk-buffer)
(set-buffer-multibyte nil)
(erase-buffer)
(insert
"#! /bin/make -f
# @file
# @brief building makefile
# @note
# This file is generated by mkcfg.el
#

include XuanWuOS.cfg
TARGET ?= XuanWuOS

MAKE_ARGS = $(strip -C $(XuanWuOS_PATH) -f xwbs/$(XuanWuOS_CFG_ELF_MK) \\
                    TARGET=$(TARGET) XuanWuOS_WKSPC_DIR=$(XuanWuOS_WKSPC_DIR))

all:
\t$(MAKE) $(MAKE_ARGS) all

clean:
\t@$(MAKE) $(MAKE_ARGS) clean

distclean:
\t@$(MAKE) $(MAKE_ARGS) distclean

.DEFAULT:
\t$(MAKE) $(MAKE_ARGS) $@

")
(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate env.rc ;;;;;;;; ;;;;;;;; ;;;;;;;;
(set-buffer xwos-envrc-buffer)
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
(insert (concat "export XuanWuOS_CFG_HOSTOS=" (symbol-name system-type) "\n"))
(insert (concat "export XuanWuOS_CFG_ARCH=" XuanWuOS-cfg-arch "\n"))
(insert (concat "export XuanWuOS_CFG_SUBARCH=" XuanWuOS-cfg-subarch "\n"))
(insert (concat "export XuanWuOS_CFG_COMPILER=" XuanWuOS-cfg-compiler "\n"))
(insert (concat "export XuanWuOS_CFG_LDSCRIPT=" XuanWuOS-cfg-lds "\n"))
(insert (concat "export XuanWuOS_CFG_CPU=" XuanWuOS-cfg-cpu "\n"))
(insert (concat "export XuanWuOS_CFG_SOC=" XuanWuOS-cfg-soc "\n"))
(insert (concat "export XuanWuOS_CFG_BRD=" XuanWuOS-cfg-brd "\n"))
(insert (concat "export XuanWuOS_CFG_CORE=" XuanWuOS-cfg-core "\n"))
(insert (concat "export XuanWuOS_CFG_XWMD=" XuanWuOS-cfg-xwmd "\n"))
(insert (concat "export XuanWuOS_CFG_XWEM=" XuanWuOS-cfg-xwem "\n"))
(insert (concat "export XuanWuOS_CFG_XWAM=" XuanWuOS-cfg-xwam "\n"))
(insert (concat "export XuanWuOS_CFG_ELF_MK=" XuanWuOS-cfg-elf-mk "\n"))
(insert (concat "export XuanWuOS_CFG_XWMO_MK=" XuanWuOS-cfg-xwmo-mk "\n"))
(insert (concat "export XuanWuOS_CFG_XWMO_PREBUILT_MK=" XuanWuOS-cfg-xwmo-prebuilt-mk "\n"))
(insert "## ******** ******** ******** ******** directory info ******** ******** ******** ******** ##\n")
(insert (concat "export XuanWuOS_PATH=" XuanWuOS-srcpath "\n"))
(insert (concat "export XuanWuOS_FWDPATH=" XuanWuOS-forward-srcpath "\n"))
(insert (concat "export XuanWuOS_BWDPATH=" XuanWuOS-backward-srcpath "\n"))
(insert (concat "export XuanWuOS_XWOS_DIR=" xwos-kr-dir "\n"))
(insert (concat "export XuanWuOS_XWCD_DIR=" xwos-cd-dir "\n"))
(insert (concat "export XuanWuOS_XWMD_DIR=" xwos-md-dir "\n"))
(insert (concat "export XuanWuOS_XWEM_DIR=" xwos-em-dir "\n"))
(insert (concat "export XuanWuOS_XWAM_DIR=" xwos-am-dir "\n"))
(insert (concat "export XuanWuOS_ARCH_DIR=" xwos-arch-dir "\n"))
(insert (concat "export XuanWuOS_CPU_DIR=" xwos-cpu-dir "\n"))
(insert (concat "export XuanWuOS_SOC_DIR=" xwos-soc-dir "\n"))
(insert (concat "export XuanWuOS_BRD_DIR=" xwos-brd-dir "\n"))
(insert (concat "export XuanWuOS_BDL_DIR=" xwos-bdl-dir "\n"))
(insert (concat "export XuanWuOS_BM_DIR=" xwos-bm-dir "\n"))
(insert (concat "export XuanWuOS_OEM_DIR=" xwos-brd-dir "/" XuanWuOS-cfg-oempath "\n"))
(insert (concat "export XuanWuOS_WKSPC_DIR=" xwos-wkspc-dir "\n"))
(insert (concat "export XuanWuOS_OBJ_DIR=" xwos-obj-dir "\n"))
(insert (concat "export XuanWuOS_OEMOBJ_DIR=" xwos-oemobj-dir "\n"))
(insert "## ******** ******** ******** ******** includes ******** ******** ******** ******** ##\n")
(insert "source ${XuanWuOS_PATH}/xwbs/util/sh/xwenv.sh\n")

(save-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
