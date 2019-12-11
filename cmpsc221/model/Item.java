package edu.psu.cmpsc221.model;

public class Item {
    public Item(String name, String lookDescription) {
        this.lookDescription = lookDescription;
        this.name = name;
    } /* end Item */

    public String getLookDescription() {
        return lookDescription;
    } /* end getLookDescription */

    public String getName() {
        return name;
    } /* end getName */

    private String lookDescription;
    private String name;
} /* end Item */
