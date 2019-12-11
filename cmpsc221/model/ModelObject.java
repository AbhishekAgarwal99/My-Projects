package edu.psu.cmpsc221.model;

import edu.psu.cmpsc221.AdventureObject;

public class ModelObject extends AdventureObject {
    protected Model getModel() {
        return Model.getInstance();
    } /* end getModel */
} /* end ModelObject */
