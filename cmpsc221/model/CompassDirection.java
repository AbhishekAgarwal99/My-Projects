package edu.psu.cmpsc221.model;

public class CompassDirection extends Direction {
    public static final String NORTH_NAME = "north";
    public static final String SOUTH_NAME = "south";
    public static final String EAST_NAME = "east";
    public static final String WEST_NAME = "west";

    public static final CompassDirection NORTH = new CompassDirection(NORTH_NAME);
    public static final CompassDirection SOUTH = new CompassDirection(SOUTH_NAME);
    public static final CompassDirection EAST = new CompassDirection(EAST_NAME);
    public static final CompassDirection WEST = new CompassDirection(WEST_NAME);

    static {
        NORTH.setOppositeCompassDirection(SOUTH);
        SOUTH.setOppositeCompassDirection(NORTH);
        EAST.setOppositeCompassDirection(WEST);
        WEST.setOppositeCompassDirection(EAST);
    } /* end static */

    public CompassDirection(String name) {
        super(name);
    } /* end CompassDirection */

    CompassDirection getOppositeCompassDirection() {
        return oppositeCompassDirection;
    } /* end setOppositeCompassDirection */

    private void setOppositeCompassDirection(CompassDirection compassDirection) {
        this.oppositeCompassDirection = compassDirection;
    } /* end setOppositeCompassDirection */

    private CompassDirection oppositeCompassDirection;
} /* end CompassDirection */
