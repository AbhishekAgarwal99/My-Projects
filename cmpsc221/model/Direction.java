package edu.psu.cmpsc221.model;

/**
 * A general direction for teh [sic] game
 */
public class Direction {
    public Direction(String name) {
        this.name = name;
    } /* end Direction */

    public String getName() {
        return name;
    } /* end getName */

    private String name;
} /* end Direction */
