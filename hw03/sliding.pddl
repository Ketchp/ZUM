(define
    (domain sliding)
    (:predicates
        (TILE  ?t)  ; is tile; ID from 1 to N^2-1
        (XPOS  ?x)  ; is X position
        (YPOS  ?y)  ; is Y position
        (EMPTY ?x ?y)  ; is EMPTY tile
        (isat  ?t ?x ?y)  ; t is tile at position (x, y)
        (isngb ?x1 ?y1 ?x2 ?y2)  ; positions (x1 y1) and (x2 y2) are neighbors
    )

    (:action slide
        :parameters (?t ?x1 ?y1 ?x2 ?y2)
        :precondition (and
            (TILE ?t)
            (XPOS ?x1) (XPOS ?x2)
            (YPOS ?y1) (YPOS ?y2)
            (isngb ?x1 ?y1 ?x2 ?y2)
            (EMPTY ?x2 ?y2)
            (isat ?t ?x1 ?y1)
        )
        :effect (and
            (EMPTY ?x1 ?y1)
            (isat ?t ?x2 ?y2)
        )
    )
)
