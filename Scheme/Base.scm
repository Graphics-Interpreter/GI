(define (and expr . args)
    (define (list-and l)
        (cond ((null? l) #t)
              ((car l) (list-and (cdr l)))
              (else #f)))
    (if expr (list-and args) #f))

(define (not expr) (if expr #f #t))

(define (= expr . args)
    (define (equal x y)
        (and (not (< x y)) (not (< y x))))
    (define (equal-list l)
        (cond ((null? (cdr l)) #t)
              ((equal (car l) (car (cdr l))) (equal-list (cdr l)))
              (else #f)))
    (if (equal expr (car args)) (equal-list args) #f))

(define (- num . args)
    (define (add-list l)
        (if (null? l) 0 (+ (car l) (add-list (cdr l)))))
    (if (null? args)
        (#opposite num)
        (+ num (#opposite (add-list args)))))

(define (/ num . args)
    (define (multiply-list l)
        (if (null? l) 1 (* (car l) (multiply-list (cdr l)))))
    (* num (#reciprocal (multiply-list args))))

(define (or expr . args)
    (define (list-or l)
        (cond ((null? l) #f)
              ((car l) #t)
              (else (list-or (cdr l)))))
    (if expr #t (list-or args)))

(define (remainder a b)
  (if (< a b) a (remainder (- a b) b)))

(define (append l1 l2)
    (if (null? l1) l2 (cons (car l1) (append (cdr l1) l2))))

(define (map seq op)
    (if (null? seq) nil (cons (op (car seq)) (map (cdr seq) op))))

(define (abs x) ((if (< 0 x) + -) x))

(define (reduce seq op)
    (define (reduce-iter seq res)
    (if (null? seq) res (reduce-iter (cdr seq) (op res (car seq)))))
    (reduce-iter seq nil))
