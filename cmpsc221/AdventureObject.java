package edu.psu.cmpsc221;

import edu.psu.cmpsc221.controller.Controller;

public class AdventureObject {
    protected Controller getController() {
        return Controller.getInstance();
    } /* end getController */
} /* end AdventureObject */
