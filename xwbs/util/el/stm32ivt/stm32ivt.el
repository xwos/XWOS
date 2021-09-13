#! /bin/sh
":"; exec emacs --quick --script "$0" -- "$@" # -*- mode: emacs-lisp; lexical-binding: t; -*-
;; @file
;; @brief Emacs-script to generate STM32 Interupt Vector Table (IVT)
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
    (princ (format "%s <stm32cube xwmo Path>\n" elname)))

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
(defvar opt-stm32cube-dir nil "stm32cube path")
(defvar opt-show-usage nil "print usage")
(logd "%s" argv)
(let ((options-done nil))
  (pop argv)  ; Remove the -- separator
  (while argv
    (let ((option (pop argv)))
      (cond
       (options-done (setq opt-stm32cube-dir option))

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

       (t (setq opt-stm32cube-dir option)))))

  (unless (> (length opt-stm32cube-dir) 0)
    (setq opt-stm32cube-dir "."))
  (unless (null opt-show-usage)
    (usage)
    (kill-emacs 0))

  (logd "opt-stm32cube-dir:%s" opt-stm32cube-dir))

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
(defvar cubemx-dir (expand-directory opt-stm32cube-dir "/cubemx/"))
(if (not (file-exists-p cubemx-dir))
    (progn
     (loge "Can't find folder:%s" cubemx-dir)
     (kill-emacs ENOENT)))
(defvar ivt-dir (expand-directory cubemx-dir "IVT/"))
(if (not (file-exists-p ivt-dir))
    (make-directory ivt-dir))
(defvar device-dir (expand-directory cubemx-dir "Drivers/CMSIS/Device/ST/"))
(if (not (file-exists-p device-dir))
    (progn
     (loge "Can't find folder:%s" device-dir)
     (kill-emacs ENOENT)))
(defvar cubemx-makefile (expand-file-name "Makefile" cubemx-dir))
(if (not (file-exists-p cubemx-makefile))
    (progn
     (loge "Can't find file:%s" cubemx-makefile)
     (kill-emacs ENOENT)))
(defvar family
  (catch 'break
    (let ((notdot)
          (family-list (directory-files device-dir)))
      (while family-list
        (setq notdot (pop family-list))
        (if (and (not (string= notdot ".")) (not (string= notdot "..")))
            (throw 'break notdot))))))
(defvar family-downcase (downcase family))
(defvar family-dir (expand-directory device-dir family "/"))
(defvar family-inc-dir (expand-directory family-dir "Include/"))
(defvar family-h (expand-file-name (concat family-downcase ".h") family-inc-dir))
(defvar family-sys-h (expand-file-name (concat "system_" family-downcase ".h") family-inc-dir))
(defvar soc-h-name
  (catch 'break
    (let ((family-h-name (file-name-nondirectory family-h))
          (family-sys-h-name (file-name-nondirectory family-sys-h))
          (soc-list (directory-files family-inc-dir))
          (soc-h-name))
      (while soc-list
        (setq soc-h-name (pop soc-list))
        (if (and (not (string= soc-h-name "."))
                 (not (string= soc-h-name ".."))
                 (not (string= soc-h-name family-h-name))
                 (not (string= soc-h-name family-sys-h-name)))
            (throw 'break soc-h-name))))))
(defvar soc-name (file-name-sans-extension soc-h-name))
(defvar soc-h (expand-file-name soc-h-name family-inc-dir))
(defvar isr-c (expand-file-name "isr.c" ivt-dir))
(defvar isr-h (expand-file-name "isr.h" ivt-dir))
(defvar ivt-c (expand-file-name "ivt.c" ivt-dir))

(logd "soc-name:%s" soc-name)
(logd "soc-h:%s" soc-h)

;;;;;;;; ;;;;;;;; ;;;;;;;; Device Macro ;;;;;;;; ;;;;;;;; ;;;;;;;;
;; Iterate over the input buffer to remove all '\r'
(defvar cubemx-makefile-buffer (find-file-noselect cubemx-makefile))
(set-buffer cubemx-makefile-buffer)
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

;; Get variable C_DEFS
(defvar cdefs-list
      (split-string
       (let ((variable "C_DEFS")
             (iterpoint 1)
             (variable-beginning 1)
             (variable-end 1)
             (variable-content ""))
         (set-buffer cubemx-makefile-buffer)
         (set-buffer-multibyte t)
         (goto-char (point-min))
         (setq iterpoint
               (re-search-forward (concat variable "[ :?]*=\\(.+\\)\\\\$") nil t))
         (if (null iterpoint)
             (progn
               (setq iterpoint
                     (re-search-forward (concat variable "[ :?]*=\\(.+\\)$") nil t))
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
             ;;(logd "%d:%d:%d" variable-beginning iterpoint (point-max))
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
                 ;;(logd "line <%d:%d>%s" line-beginning line-end line-content)
                 (setq variable-content (concat variable-content line-content))
                 (setq variable-end line-end)
                 (setq iterpoint line-beginning)
                 (goto-char iterpoint)))))
         ;;(logd "variable <%d:%d>%s" variable-beginning variable-end variable-content)
         variable-content)))
;;(logd "cdefs-list:%s" cdefs-list)
(defvar soc-macro
  (substring
   (catch 'break
     (let (element))
     (while cdefs-list
       (setq element (pop cdefs-list))
       (if (string= (upcase element) (concat "-D" (upcase soc-name)))
           (throw 'break element)))) 2 -1))
(logd "soc-macro:%s" soc-macro)
(kill-buffer cubemx-makefile-buffer)

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
     "void \\1_IRQHandler(void) __xwcc_weakalias(\"MX_NOOP_IRQHandler\");" t)))
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
