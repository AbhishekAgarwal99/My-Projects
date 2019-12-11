package edu.psu.cmpsc221.view;

public interface View {
    public String getUserCommand();
    public void inventory();
    public void look();
    public void processCantGoDirection(String directionName);
    public void processDropItemNotInInventory(String itemName);
    public void processDropSuccessful(String itemName);
    public void processGetItemNotInInventory(String itemName);
    public void processGetSuccessful(String itemName);
    public void processInventoryFullException(String message);
    public void processQuitCommand();
    public void processUnknownCommand(String command);
} /* end View */
