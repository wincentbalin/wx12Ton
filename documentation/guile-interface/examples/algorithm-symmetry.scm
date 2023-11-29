;Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>
;This file is part of wx12Ton: http://wx12ton.sourceforge.net/
;
;This is an example for a algorithm file.

(define (info) '( 
 ("pretty-ret"      ("en" "symmetric retrograde")
                    ("de" "Symetrischer Krebs")
 )
 ("self-retrograde" ("en" "mirror symmetric (t,m)")
                    ("de" "Spiegelsymmetrisch (t,m)")
 )
))

(define (pretty-ret s t m)
 (series_retrograde s (note_interval (series_at s 0) (series_at s 11)))
)

(define (self-retrograde s t m)  
 (if (not(eq? m 0)) (begin
  (while (or 
          (not(series-equal? s (pretty-ret s 0 0)))
          (not(equal? (cdr (series_find_error s t)) "OK"))
         )
  
   (if (> m 0)
    (set! s (series_find_valid_forw (series_next_permutation s) t))
    (set! s (series_find_valid_backw (series_prev_permutation s) t))
   )
  )
 ))
 s
)
