package edu.psu.cmpsc221.model;

public class Map {
   public Map() {
      thomas101 =
         new Room("You are seated in 101 Thomas Building.  It is brightly lit and is the site of your favourite class OF ALL TIME.  Evaaaar.",
                 "101Thomas");
      thomasHallway = new Room("You are in the main hallway on the first floor of Thomas Building.", "ThomasFirstHallway");
      thomasEastExit = new Room("You are standing outside Thomas Builindg along Pollock Road", "thomasEastExit");
      mcElwainCourtyard = new Room("You are in McElwain's courtyard, a peaceful beautiful setting reminiscent of earlier times.", "(mcElwain");

      thomas101.addCompassExit(CompassDirection.SOUTH, thomasHallway);
      thomasHallway.addCompassExit(CompassDirection.EAST, thomasEastExit);
      thomasEastExit.addCompassExit(CompassDirection.EAST, mcElwainCourtyard);

      thomas101.addToInventory(new Item("ball", "A fully inflated beach ball lies nearby."));
      thomas101.addToInventory(new Item("pen", "A totally dried up dry-write pen is discarded here."));
      thomas101.addToInventory(new Item("laptop", "There is a laptop here."));
      thomas101.addToInventory(new Item("books", "A large pile of computer science books sits on a desk."));
      thomas101.addToInventory(new Item("bookbag", "A rather heavy bookbag sits unattended in the room."));
      thomas101.addToInventory(new Item("homework", "A thick pile of unclaimed homework sits on the front table."));
   } /* end Map */

   public Room getInitialPlayerRoom() {
      return thomas101;
   } /* end getInitialPlayerRoom */

   private Room thomas101;
   private Room thomasHallway;
   private Room thomasEastExit;
   private Room mcElwainCourtyard;
} /* end Map */
