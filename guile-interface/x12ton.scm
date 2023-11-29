#!Copyright (C) 2004 Matthias Kleinmann <Matthias dot Kleinmann at gmx dot de>
!#

(define-module (site x12ton))

(export note_new)
(export note_val)
(export note_oct)
(export note_set_no_oct)
(export interval_modulo)
(export note_interval)
(export note_transpose)

(export series_new)
(export series_to_string)
(export series_at)
(export series_basic_form)
(export series_retrograde)
(export series_inversion)
(export series_normalized)
(export series_next_permutation)
(export series_prev_permutation)
(export series_find_valid_forw)
(export series_find_valid_backw)
(export series_find_error)
(export series_find_doubled)
(export error_pos_transform)
(export series_read)
(export series_write)

(export note-oct?)
(export note-less?)
(export note-equal?)
(export note-eq?)

(export series-normalized?)
(export series-less?)
(export series-equal?)
(export series-eq?)


(load-extension "libguile-12ton" "init_12ton")
