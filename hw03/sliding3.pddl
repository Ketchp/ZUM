(define
    (problem sliding3)
    (:domain sliding)

    (:objects
        x1 x2 x3 y1 y2 y3
        t1 t2 t3 t4 t5 t6 t7 t8
    )
    (:init
        (TILE t1) (TILE t2) (TILE t3)
        (TILE t4) (TILE t5) (TILE t6)
        (TILE t7) (TILE t8)
        (XPOS x1) (XPOS x2) (XPOS x3)
        (YPOS y1) (YPOS y2) (YPOS y3)
        
        (isat t1 x1 y1) (isat t2 x2 y1) (isat t3 x3 y1)
        (isat t4 x1 y2) (EMPTY   x2 y2) (isat t5 x3 y2)
        (isat t7 x1 y3) (isat t8 x2 y3) (isat t6 x3 y3)
        
		(isngb x1 y1 x1 y2)
		(isngb x1 y1 x2 y1)
		(isngb x1 y2 x1 y1)
		(isngb x1 y2 x1 y3)
		(isngb x1 y2 x2 y2)
		(isngb x1 y3 x1 y2)
		(isngb x1 y3 x2 y3)
		(isngb x2 y1 x1 y1)
		(isngb x2 y1 x2 y2)
		(isngb x2 y1 x3 y1)
		(isngb x2 y2 x1 y2)
		(isngb x2 y2 x2 y1)
		(isngb x2 y2 x2 y3)
		(isngb x2 y2 x3 y2)
		(isngb x2 y3 x1 y3)
		(isngb x2 y3 x2 y2)
		(isngb x2 y3 x3 y3)
		(isngb x3 y1 x2 y1)
		(isngb x3 y1 x3 y2)
		(isngb x3 y2 x2 y2)
		(isngb x3 y2 x3 y1)
		(isngb x3 y2 x3 y3)
		(isngb x3 y3 x2 y3)
		(isngb x3 y3 x3 y2)
    )
    (:goal (and
            (isat t1 x1 y1) (isat t2 x2 y1) (isat t3 x3 y1)
            (isat t4 x1 y2) (isat t5 x2 y2) (isat t6 x3 y2)
            (isat t7 x1 y3) (isat t8 x2 y3) (EMPTY   x3 y3)
        )
    )
)

