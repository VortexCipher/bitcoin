(use-modules (gnu packages)
             (gnu packages autotools)
             ((gnu packages bash) #:select (bash-minimal))
             (gnu packages bison)
             ((gnu packages certs) #:select (nss-certs))
             ((gnu packages cmake) #:select (cmake-minimal))
             (gnu packages commencement)
             (gnu packages compression)
             (gnu packages cross-base)
             (gnu packages file)
             (gnu packages gawk)
             (gnu packages gcc)
             ((gnu packages installers) #:select (nsis-x86_64))
             ((gnu packages linux) #:select (linux-libre-headers-6.1 util-linux))
             (gnu packages llvm)
             (gnu packages mingw)
             (gnu packages moreutils)
             (gnu packages pkg-config)
             ((gnu packages python) #:select (python-minimal))
             ((gnu packages python-build) #:select (python-tomli))
             ((gnu packages python-crypto) #:select (python-asn1crypto))
             ((gnu packages tls) #:select (openssl))
             ((gnu packages version-control) #:select (git-minimal))
             (guix build-system cmake)
             (guix build-system gnu)
             (guix build-system python)
             (guix build-system trivial)
             (guix download)
             (guix gexp)
             (guix git-download)
             ((guix licenses) #:prefix license:)
             (guix packages)
             ((guix utils) #:select (substitute-keyword-arguments)))

(define-syntax-rule (search-our-patches file-name ...)
  "Return the list of absolute file names corresponding to each
FILE-NAME found in ./patches relative to the current file."
  (parameterize
      ((%patch-path (list (string-append (dirname (current-filename)) "/patches"))))
    (list (search-patch file-name) ...)))

(define building-on (string-append "--build=" (list-ref (string-split (%current-system) #\-) 0) "-guix-linux-gnu"))

(define (make-cross-toolchain target
                              base-gcc-for-libc
                              base-kernel-headers
                              base-libc
                              base-gcc)
  "Create a cross-compilation toolchain package for TARGET"
  (let* ((xbinutils (cross-binutils target))
         ;; 1. Build a cross-compiling gcc without targeting any libc, derived
         ;; from BASE-GCC-FOR-LIBC
         (xgcc-sans-libc (cross-gcc target
                                    #:xgcc base-gcc-for-libc
                                    #:xbinutils xbinutils))
