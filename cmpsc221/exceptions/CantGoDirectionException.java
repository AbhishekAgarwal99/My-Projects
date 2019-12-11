package edu.psu.cmpsc221.exceptions;

import edu.psu.cmpsc221.model.Direction;

public class CantGoDirectionException extends Exception {
    public CantGoDirectionException(Direction direction) {
        super(direction.getName());
    } /* end CantGoDirectionException */
} /* end CantGoDirectionException */
