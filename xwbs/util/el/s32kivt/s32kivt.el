#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to generate S32K Interupt Vector Table (IVT)
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
(setq debug-on-error t)

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

;;;;;;;; ;;;;;;;; ;;;;;;;; misc function ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defun usage ()
    (princ (format "%s <S32DS Project Path>\n" elname)))

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
(defvar opt-s32k-prjdir nil "S32k Project path")
(defvar opt-show-usage nil "print usage")
(logd "%s" argv)
(let ((options-done nil))
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      (cond
       (options-done (setq opt-s32k-prjdir option))

       ;; options after "--"
       ((string= option "--") (setq options-done t))

       ;; --help
       ;; -h
       ((string= option "--help") (setq opt-show-usage t))
       ((string= option "-h") (setq opt-show-usage t))

       ;; Unknown options
       ((string-prefix-p "--" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       ((string-prefix-p "-" option)
        (loge "Unknown option: %s" option)
        (kill-emacs EINVAL))

       (t (setq opt-s32k-prjdir option)))))

  (unless (> (length opt-s32k-prjdir) 0)
    (setq opt-s32k-prjdir "."))
  (unless (null opt-show-usage)
    (usage)
    (kill-emacs 0))

  (logd "opt-s32k-prjdir:%s" opt-s32k-prjdir))

;;;;;;;; ;;;;;;;; ;;;;;;;; Path ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defvar template-dir (expand-directory elpath "template/"))
(defvar isr-c-t (expand-file-name "isr.c.t" template-dir))
(if (not (file-exists-p isr-c-t))
    (progn
     (loge "Can't find file:template/isr.c.t")
     (kill-emacs ENOENT)))
(defvar isr-h-t (expand-file-name "isr.h.t" template-dir))
(if (not (file-exists-p isr-h-t))
    (progn
     (loge "Can't find file:template/isr.h.t")
     (kill-emacs ENOENT)))
(defvar ivt-c-t (expand-file-name "ivt.c.t" template-dir))
(if (not (file-exists-p isr-c-t))
    (progn
     (loge "Can't find file:template/ivt.c.t")
     (kill-emacs ENOENT)))
(defvar sdk-dir (expand-directory opt-s32k-prjdir "/SDK/"))
(if (not (file-exists-p sdk-dir))
    (progn
     (loge "Can't find folder:%s" sdk-dir)
     (kill-emacs ENOENT)))
(defvar device-dir (expand-directory sdk-dir "platform/devices/"))
(if (not (file-exists-p device-dir))
    (progn
     (loge "Can't find folder:%s" device-dir)
     (kill-emacs ENOENT)))
(defvar s32kbsp-dir (expand-directory opt-s32k-prjdir "/brd/bm/s32kbsp/"))
(if (not (file-exists-p s32kbsp-dir))
    (progn
     (loge "Can't find folder:%s" s32kbsp-dir)
     (kill-emacs ENOENT)))
(defvar ivt-dir (expand-directory s32kbsp-dir "xwac/ivt/"))
(if (not (file-exists-p ivt-dir))
    (make-directory ivt-dir))
(defvar soc
  (catch 'break
    (let ((notdot)
          (soc-list (directory-files device-dir nil "S32K.+")))
      (while soc-list
        (setq notdot (pop soc-list))
        (if (and (not (string= notdot ".")) (not (string= notdot "..")))
            (throw 'break notdot))))))
(defvar soc-dir (expand-directory device-dir soc "/"))
(defvar soc-inc-dir (expand-directory soc-dir "include/"))
(defvar soc-h (expand-file-name (concat soc ".h") soc-inc-dir))

(defvar isr-c (expand-file-name "isr.c" ivt-dir))
(defvar isr-h (expand-file-name "isr.h" ivt-dir))
(defvar ivt-c (expand-file-name "ivt.c" ivt-dir))

(logd "soc:%s" soc)
(logd "soc-h:%s" soc-h)

;;;;;;;; ;;;;;;;; ;;;;;;;; IRQ Table ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defvar soc-h-buffer (find-file-noselect soc-h))
(set-buffer soc-h-buffer)
(set-buffer-multibyte t)
(goto-char (point-min))

(defvar irq-table-beginning (search-forward "SysTick_IRQn" nil t))
(if (null irq-table-beginning) (kill-emacs EFAULT))
(setq irq-table-beginning (re-search-forward
                             "  \\([A-Z_a-z0-9]+\\)\\(_IRQn\\) += [0-9 ]+.+" nil t))
(setq irq-table-beginning (line-beginning-position))

(defvar irq-table-end (search-forward "} IRQn_Type;" nil t))
(if (null irq-table-end) (kill-emacs EFAULT))
(forward-line -1)
(setq irq-table-end (line-end-position))

(defvar irq-table (buffer-substring irq-table-beginning irq-table-end))
(logi "irq-table:\n%s" irq-table)
(kill-buffer soc-h-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate isr.c ;;;;;;;; ;;;;;;;; ;;;;;;;;
(copy-file isr-c-t isr-c t)
(defvar isr-c-buffer (find-file-noselect isr-c))
(set-buffer isr-c-buffer)
(set-buffer-multibyte t)

(goto-char (point-min))
(search-forward "[T:ISR]" nil t)
(replace-match irq-table t)

(goto-char (point-min))
(let ((iterpoint))
  (while (setq iterpoint
               (re-search-forward
                "  \\([A-Z_a-z0-9]+\\)\\(_IRQn\\) += [0-9 ]+.+" nil t))
    (replace-match
     "void \\1_IRQHandler(void) __xwcc_weakalias(\"S32KBSP_NOOP_IRQHandler\");" t)))
(save-buffer)
(kill-buffer isr-c-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate isr.h ;;;;;;;; ;;;;;;;; ;;;;;;;;
(copy-file isr-h-t isr-h t)
(defvar isr-h-buffer (find-file-noselect isr-h))
(set-buffer isr-h-buffer)
(set-buffer-multibyte t)

(goto-char (point-min))
(search-forward "[T:ISR]" nil t)
(replace-match irq-table t)

(goto-char (point-min))
(let ((iterpoint))
  (while (setq iterpoint
               (re-search-forward
                "  \\([A-Z_a-z0-9]+\\)\\(_IRQn\\) += [0-9 ]+.+" nil t))
    (replace-match "void \\1_IRQHandler(void);" t)))

(save-buffer)
(kill-buffer isr-h-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; generate ivt.c ;;;;;;;; ;;;;;;;; ;;;;;;;;
(copy-file ivt-c-t ivt-c t)
(defvar ivt-c-buffer (find-file-noselect ivt-c))
(set-buffer ivt-c-buffer)
(set-buffer-multibyte t)

(goto-char (point-min))
(search-forward "[T:VECTOR]" nil t)
(replace-match irq-table t)

(goto-char (point-min))
(let ((iterpoint))
  (while (setq iterpoint
               (re-search-forward
                "  \\([A-Z_a-z0-9]+\\)\\(_IRQn\\) += [0-9 ]+.+" nil t))
    (replace-match "                [\\1\\2] = \\1_IRQHandler," t)))

(save-buffer)
(kill-buffer ivt-c-buffer)

;;;;;;;; ;;;;;;;; ;;;;;;;; exit ;;;;;;;; ;;;;;;;; ;;;;;;;;
(kill-emacs 0)
